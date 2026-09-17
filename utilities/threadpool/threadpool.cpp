#include <pch/pch.hpp>
#include <cstring>
#include <exception>
#include <new>
#include <thread>
#include <limits>
#include <stdexcept>
#include <utilities/memory/memory.hpp>
#include <utilities/diag.hpp>
#include <utilities/performance.hpp>
#include <protection/game_addresses.hpp>
#include "threadpool.hpp"
#include "work_queue.hpp"

namespace threadpool {

	namespace detail {

		namespace offsets {

			constexpr std::size_t refcount {0x08};
			constexpr std::size_t status {0x10};
			constexpr std::size_t flags_word {0x14};
			constexpr std::size_t priority {0x15};
			constexpr std::size_t name_set_flag {0x16};
			constexpr std::size_t pool_owner {0x18};
			constexpr std::size_t completion_event {0x20};
			constexpr std::size_t completion_counter {0x28};
			constexpr std::size_t name_buffer {0x30};
			constexpr std::size_t sbo_buffer {0x50};
			constexpr std::size_t callable_impl {0x88};

		} // namespace offsets

		constexpr std::size_t k_job_alloc_size {144};
		constexpr std::size_t k_name_buffer_len {31};
		constexpr std::size_t k_sbo_buffer_size {56};
		constexpr std::size_t k_vti_add_job {16};

		// This bridge requires the x64 MSVC std::function layout used by tier0.
		static_assert (sizeof (std::function<void ()>) ==
			k_job_alloc_size - offsets::sbo_buffer);
		static_assert (offsets::sbo_buffer % alignof (std::function<void ()>) == 0);

		inline std::uintptr_t std_function_job_vtable {0};
		inline std::uintptr_t g_mem_alloc {};
		inline pool g_pool {};

	} // namespace detail

	job::job (std::uintptr_t ptr, bool add_reference) : m_ptr (ptr) {
		if (this->m_ptr && add_reference) {
			this->add_ref ();
		}
	}

	job::~job () {
		if (this->m_ptr) {
			this->do_release ();
		}
	}

	job::job (const job& o) : m_ptr (o.m_ptr) {
		if (this->m_ptr) {
			this->add_ref ();
		}
	}

	job& job::operator=(const job& o) {
		if (this != &o) {
			if (this->m_ptr) {
				this->do_release ();
			}

			this->m_ptr = o.m_ptr;

			if (this->m_ptr) {
				this->add_ref ();
			}
		}

		return *this;
	}

	job::job (job&& o) noexcept : m_ptr (o.m_ptr) {
		o.m_ptr = 0;
	}

	job& job::operator=(job&& o) noexcept {
		if (this != &o) {
			if (this->m_ptr) {
				this->do_release ();
			}

			this->m_ptr = o.m_ptr;
			o.m_ptr = 0;
		}

		return *this;
	}

	job_status job::status () const {
		return static_cast<job_status>(*reinterpret_cast<volatile int*>(this->m_ptr + detail::offsets::status));
	}

	bool job::complete () const {
		return this->status () == job_status::idle;
	}

	void job::wait () const {
		if (!this->m_ptr) {
			return;
		}

		auto event_pp = *reinterpret_cast<HANDLE**>(this->m_ptr + detail::offsets::completion_event);
		if (event_pp && *event_pp) {
			WaitForSingleObject (*event_pp, INFINITE);
			return;
		}

		auto counter = *reinterpret_cast<volatile int**>(this->m_ptr + detail::offsets::completion_counter);
		if (counter) {
			while (this->status () != job_status::idle) {
				auto snapshot {*counter};
				WaitOnAddress ((volatile void*) counter, &snapshot, sizeof (int), 1);
			}

			return;
		}

		while (!this->complete ()) {
			_mm_pause ();
		}
	}

	void job::add_ref () const {
		memory::call_vfunc<long> (this->m_ptr, 1);
	}

	void job::do_release () const {
		memory::call_vfunc<long> (this->m_ptr, 2);
	}

	void pool::add_job (std::uintptr_t j) const {
		memory::call_vfunc<void> (this->m_instance, detail::k_vti_add_job, j);
	}

	std::uintptr_t make_job (std::function<void ()>&& func, job_priority priority, const char* debug_name) {
		// CStdFunctionJob::Release destroys this allocation in tier0. Do not
		// use our overridden new[]: it returns a pointer past a private header,
		// which is not a valid allocation base for the engine's Free.
		auto raw = reinterpret_cast<std::uintptr_t>(memory::call_vfunc<void*> (
			detail::g_mem_alloc, 1, detail::k_job_alloc_size));
		if (!raw) {
			throw std::bad_alloc ();
		}
		std::memset (reinterpret_cast<void*>(raw), 0, detail::k_job_alloc_size);

		*reinterpret_cast<std::uintptr_t*>(raw) = detail::std_function_job_vtable;
		*reinterpret_cast<std::int32_t*>(raw + detail::offsets::refcount) = 1;
		*reinterpret_cast<volatile int*>(raw + detail::offsets::status) = static_cast<int>(job_status::reset);
		*reinterpret_cast<std::uint8_t*>(raw + detail::offsets::priority) = static_cast<std::uint8_t>(priority);

		// Move-construct in place so non-trivial small callables are moved
		// correctly instead of byte-relocating their internal objects.
		::new (reinterpret_cast<void*>(raw + detail::offsets::sbo_buffer))
			std::function<void ()> (std::move (func));

		if (debug_name) {
			auto dst = reinterpret_cast<char*>(raw + detail::offsets::name_buffer);
			std::size_t i {0};

			while (i < detail::k_name_buffer_len - 1 && debug_name [i]) {
				dst [i] = debug_name [i];
				++i;
			}

			dst [i] = '\0';

			*reinterpret_cast<std::uint8_t*> (raw + detail::offsets::name_set_flag) = 1;
		}

		return raw;
	}

	namespace native {

		struct alignas(64) worker_slot
		{
			HANDLE thread_handle{};
			int worker_index{};
			std::atomic<std::uint32_t> task_gen{};
			std::atomic<std::uint32_t> done_gen{};
			const std::function<void(int)>* body{};
			std::exception_ptr error{};
		};

		// A batch owns the slots until every participant has joined. Lifecycle
		// operations use the same mutex, so callbacks cannot outlive their inputs.
		inline std::mutex g_dispatch_mutex;
		inline std::atomic<bool> g_shutdown{false};
		inline bool g_initialized{};
		inline std::atomic<int> g_total_threads{1};
		inline std::vector<std::unique_ptr<worker_slot>> g_workers;

		struct dispatch_tls
		{
			std::atomic<DWORD> index{TLS_OUT_OF_INDEXES};
			~dispatch_tls()
			{
				const auto slot = index.load();
				if (slot != TLS_OUT_OF_INDEXES) TlsFree(slot);
			}
		};
		inline dispatch_tls g_tls;

		inline bool in_dispatch() noexcept
		{
			const auto slot = g_tls.index.load(std::memory_order_acquire);
			return slot != TLS_OUT_OF_INDEXES && TlsGetValue(slot) != nullptr;
		}

		// Dynamic TLS is required by this project's manual-mapped module.
		struct dispatch_scope
		{
			DWORD slot;
			void* previous;
			dispatch_scope() : slot(g_tls.index.load(std::memory_order_acquire)), previous(TlsGetValue(slot))
			{
				if (!TlsSetValue(slot, this)) throw std::runtime_error("threadpool TLS setup failed");
			}
			~dispatch_scope() { TlsSetValue(slot, previous); }
		};

		inline DWORD WINAPI worker_thread_proc(LPVOID param)
		{
			auto& worker = *static_cast<worker_slot*>(param);
			// Normal priority: do not preempt the game's render/input threads.
			for (;;)
			{
				auto generation = worker.task_gen.load(std::memory_order_acquire);
				while (generation == worker.done_gen.load(std::memory_order_relaxed))
				{
					worker.task_gen.wait(generation);
					generation = worker.task_gen.load(std::memory_order_acquire);
				}
				if (g_shutdown.load(std::memory_order_acquire)) return 0;
				try
				{
					dispatch_scope scope;
					(*worker.body)(worker.worker_index);
				}
				catch (...) { worker.error = std::current_exception(); }
				worker.done_gen.store(generation, std::memory_order_release);
				worker.done_gen.notify_one();
			}
		}

		// Caller holds g_dispatch_mutex. Publish initialization only after creation.
		inline void init_workers()
		{
			if (g_initialized) return;
			if (g_tls.index.load() == TLS_OUT_OF_INDEXES)
			{
				const auto slot = TlsAlloc();
				if (slot == TLS_OUT_OF_INDEXES) throw std::runtime_error("threadpool TLS allocation failed");
				g_tls.index.store(slot, std::memory_order_release);
			}
			auto hw = GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);
			if (!hw) hw = std::thread::hardware_concurrency();
			hw = std::max<DWORD>(hw, 1);
			const auto total = static_cast<int>(std::min<DWORD>(hw, std::numeric_limits<int>::max()));
			// Allocate all slots before starting any threads (exception-safe setup).
			g_workers.clear();
			g_workers.reserve(static_cast<std::size_t>(total - 1));
			for (int i = 1; i < total; ++i)
			{
				auto worker = std::make_unique<worker_slot>();
				worker->worker_index = i;
				g_workers.push_back(std::move(worker));
			}
			g_shutdown.store(false, std::memory_order_release);
			for (std::size_t i = 0; i < g_workers.size(); ++i)
			{
				auto& worker = *g_workers[i];
				worker.thread_handle = CreateThread(nullptr, 0, worker_thread_proc, &worker, 0, nullptr);
				if (!worker.thread_handle)
				{
					// Never wait for a worker that failed to start.
					g_workers.resize(i);
					diag::write(diag::level::warning, "threadpool: worker creation failed; using available workers");
					break;
				}
			}
			g_total_threads.store(static_cast<int>(g_workers.size()) + 1, std::memory_order_release);
			g_initialized = true;
		}

		inline void stop_workers() noexcept
		{
			// Shutdown is an owner/lifecycle operation, never a worker callback.
			if (in_dispatch()) return;
			std::lock_guard lock(g_dispatch_mutex);
			if (!g_initialized) return;
			g_shutdown.store(true, std::memory_order_release);
			for (auto& worker : g_workers)
			{
				worker->task_gen.fetch_add(1, std::memory_order_release);
				worker->task_gen.notify_one();
			}
			for (auto& worker : g_workers)
			{
				// A timeout must not free a slot or unload code still in use.
				WaitForSingleObject(worker->thread_handle, INFINITE);
				CloseHandle(worker->thread_handle);
			}
			g_workers.clear();
			g_total_threads.store(1, std::memory_order_release);
			g_initialized = false;
		}
	} // namespace native

	bool initialize () {
		std::lock_guard lock(native::g_dispatch_mutex);
		native::init_workers ();

		const auto tier0 = MODULE_BASE ("tier0.dll");
		if (tier0) {
			const auto allocator_export = MODULE_EXPORT ("tier0.dll:g_pMemAlloc");
			if (allocator_export) {
				detail::g_mem_alloc = memory::read<std::uintptr_t> (allocator_export);
			}

			const auto threadpool_export = MODULE_EXPORT ("tier0.dll:g_pThreadPool");
			if (threadpool_export) {
				detail::g_pool = pool (memory::read<std::uintptr_t> (threadpool_export));
			}

			detail::std_function_job_vtable = memory::find_vtable_by_rtti (tier0, xs ("CStdFunctionJob"));
		}

		return true;
	}

	void shutdown () noexcept {
		native::stop_workers ();
	}

	int get_thread_count () noexcept {
		if (native::in_dispatch()) return native::g_total_threads.load(std::memory_order_acquire);
		try {
			std::lock_guard lock(native::g_dispatch_mutex);
			native::init_workers();
		} catch (...) {
			return 1;
		}
		return native::g_total_threads.load(std::memory_order_acquire);
	}

	job run (std::function<void ()> func, job_priority priority) {
		auto raw = make_job (std::move (func), priority);
		if (!raw) {
			return {};
		}

		job handle (raw, false);
		detail::g_pool.add_job (raw);
		return handle;
	}

	void run_sync (std::function<void ()> func, job_priority priority) {
		auto handle = run (std::move (func), priority);
		if (handle) {
			handle.wait ();
		}
	}

	void parallel_for_indexed (int begin, int end, const std::function<void (int, int, int)>& body, int min_chunk_size) {
		if (begin >= end || !body) return;
		// Nested work stays on its current thread rather than overwriting the
		// parent batch's slots or deadlocking on the dispatch mutex.
		if (native::in_dispatch()) {
			body(begin, end, 0);
			return;
		}

		utilities::performance::scope dispatch_wait{utilities::performance::stage::pool_dispatch_wait};
		std::unique_lock lock(native::g_dispatch_mutex);
		dispatch_wait.finish();
		native::init_workers();
		native::dispatch_scope scope;
		const auto grain = std::max(min_chunk_size, 1);
		const auto total = static_cast<std::int64_t>(end) - begin;
		const auto chunks = (total + grain - 1) / grain;
		const auto participants = static_cast<int>(std::min<std::int64_t>(chunks, native::g_total_threads.load()));
		if (participants <= 1) {
			body(begin, end, 0);
			return;
		}

		detail::work_queue queue(begin, end, grain);
		const std::function<void(int)> consume = [&](int id) {
			detail::work_queue::chunk chunk{};
			while (queue.claim(chunk)) body(chunk.begin, chunk.end, id);
		};
		for (int i = 1; i < participants; ++i) {
			auto& worker = *native::g_workers[static_cast<std::size_t>(i - 1)];
			worker.error = nullptr;
			worker.body = &consume;
			worker.task_gen.fetch_add(1, std::memory_order_release);
			worker.task_gen.notify_one();
		}

		std::exception_ptr error;
		try { consume(0); }
		catch (...) { error = std::current_exception(); }
		{
			utilities::performance::scope join_wait{utilities::performance::stage::pool_join_wait};
			for (int i = 1; i < participants; ++i) {
				auto& worker = *native::g_workers[static_cast<std::size_t>(i - 1)];
				const auto expected = worker.task_gen.load(std::memory_order_relaxed);
				auto done = worker.done_gen.load(std::memory_order_acquire);
				while (done != expected) {
					worker.done_gen.wait(done);
					done = worker.done_gen.load(std::memory_order_acquire);
				}
				worker.body = nullptr;
				if (!error && worker.error) error = worker.error;
			}
		}
		// Join first even when the calling-thread callback throws: its captured
		// stack objects remain alive until every worker has finished.
		if (error) std::rethrow_exception(error);
	}

	void parallel_for (int begin, int end, const std::function<void (int, int)>& body, int min_chunk_size, job_priority /*priority*/) {
		parallel_for_indexed (begin, end, [&body] (int b, int e, int /*tid*/) {
			body (b, e);
		}, min_chunk_size);
	}

	void run_batch (std::span<std::function<void ()>> tasks, job_priority priority) {
		if (tasks.empty ()) {
			return;
		}

		std::vector<job> jobs;
		jobs.reserve (tasks.size ());

		for (auto& task : tasks) {
			jobs.push_back (run (std::move (task), priority));
		}

		for (auto& j : jobs) {
			j.wait ();
		}
	}

} // namespace threadpool