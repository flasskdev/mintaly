#include <pch/pch.hpp>
#include <cstring>
#include <exception>
#include <new>
#include <thread>
#include <utilities/memory/memory.hpp>
#include <protection/game_addresses.hpp>
#include "threadpool.hpp"
#include "partition.hpp"

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
			HANDLE thread_handle{ nullptr };
			DWORD thread_id{ 0 };
			int worker_index{ 0 }; // 1-based index (main calling thread is 0)

			std::atomic<uint32_t> task_gen{ 0 };
			std::atomic<uint32_t> done_gen{ 0 };

			int chunk_begin{ 0 };
			int chunk_end{ 0 };
			const std::function<void(int, int, int)>* body{ nullptr };
		};

		inline std::atomic<bool> g_shutdown{ false };
		inline std::atomic<bool> g_initialized{ false };
		inline int g_total_threads{ 1 };
		inline int g_num_workers{ 0 };
		inline std::array<worker_slot, 64> g_workers{};

		inline DWORD WINAPI worker_thread_proc(LPVOID param)
		{
			auto* worker = static_cast<worker_slot*>(param);
			SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

			while (!g_shutdown.load(std::memory_order_relaxed))
			{
				uint32_t current_task = worker->task_gen.load(std::memory_order_acquire);
				while (current_task == worker->done_gen.load(std::memory_order_relaxed))
				{
					worker->task_gen.wait(current_task);
					if (g_shutdown.load(std::memory_order_relaxed))
						return 0;
					current_task = worker->task_gen.load(std::memory_order_acquire);
				}

				if (worker->body && *worker->body)
				{
					try
					{
						(*worker->body)(worker->chunk_begin, worker->chunk_end, worker->worker_index);
					}
					catch (...)
					{
					}
				}

				worker->done_gen.store(current_task, std::memory_order_release);
				worker->done_gen.notify_one();
			}

			return 0;
		}

		inline void init_workers()
		{
			if (g_initialized.exchange(true))
				return;

			g_shutdown.store(false, std::memory_order_relaxed);
			unsigned int hw = std::thread::hardware_concurrency();
			if (hw == 0) hw = 4;
			g_total_threads = static_cast<int>(std::clamp(hw, 1u, 64u));
			g_num_workers = g_total_threads - 1;

			for (int i = 0; i < g_num_workers; ++i)
			{
				auto& w = g_workers[i];
				w.worker_index = i + 1; // 1-based (calling thread is 0)
				w.task_gen.store(0, std::memory_order_relaxed);
				w.done_gen.store(0, std::memory_order_relaxed);
				w.thread_handle = CreateThread(nullptr, 0, worker_thread_proc, &w, 0, &w.thread_id);
			}
		}

		inline void stop_workers() noexcept
		{
			if (!g_initialized.exchange(false))
				return;

			g_shutdown.store(true, std::memory_order_release);
			for (int i = 0; i < g_num_workers; ++i)
			{
				auto& w = g_workers[i];
				w.task_gen.fetch_add(1, std::memory_order_release);
				w.task_gen.notify_all();
			}

			for (int i = 0; i < g_num_workers; ++i)
			{
				auto& w = g_workers[i];
				if (w.thread_handle)
				{
					WaitForSingleObject(w.thread_handle, 500);
					CloseHandle(w.thread_handle);
					w.thread_handle = nullptr;
				}
			}
		}

	} // namespace native

	bool initialize () {
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
		native::init_workers ();
		return native::g_total_threads;
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
		if (begin >= end) {
			return;
		}

		native::init_workers ();

		const auto total = end - begin;
		const auto max_chunks = std::min (native::g_total_threads, static_cast<int> ((total + min_chunk_size - 1) / min_chunk_size));
		if (max_chunks <= 1) {
			body (begin, end, 0);
			return;
		}

		const auto partition = detail::partition_range (begin, end, min_chunk_size, max_chunks);
		if (partition.count <= 1) {
			body (begin, end, 0);
			return;
		}

		// Dispatch chunks 1..count-1 to background workers
		for (int i = 1; i < partition.count; ++i) {
			auto& w = native::g_workers [i - 1];
			w.chunk_begin = partition.chunks [i].begin;
			w.chunk_end = partition.chunks [i].end;
			w.body = &body;
			w.task_gen.fetch_add (1, std::memory_order_release);
			w.task_gen.notify_one ();
		}

		// Calling thread executes chunk 0 (thread_id = 0)
		body (partition.chunks [0].begin, partition.chunks [0].end, 0);

		// Wait for all dispatched workers to complete
		for (int i = 1; i < partition.count; ++i) {
			auto& w = native::g_workers [i - 1];
			const uint32_t expected = w.task_gen.load (std::memory_order_relaxed);
			while (w.done_gen.load (std::memory_order_acquire) != expected) {
				w.done_gen.wait (expected - 1);
			}
		}
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