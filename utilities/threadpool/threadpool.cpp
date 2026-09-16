#include <pch/pch.hpp>
#include <cstring>
#include <exception>
#include <new>
#include <thread>
#include <utilities/memory/memory.hpp>
#include <protection/game_addresses.hpp>
#include "threadpool.hpp"

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

	bool initialize () {
		const auto tier0 = MODULE_BASE ("tier0.dll");
		if (!tier0) {
			return false;
		}

		const auto allocator_export = MODULE_EXPORT ("tier0.dll:g_pMemAlloc");
		if (!allocator_export) {
			return false;
		}
		detail::g_mem_alloc = memory::read<std::uintptr_t> (allocator_export);
		if (!detail::g_mem_alloc) {
			return false;
		}

		detail::g_pool = pool (memory::read<std::uintptr_t> (MODULE_EXPORT ("tier0.dll:g_pThreadPool")));
		if (!detail::g_pool.get ()) {
			return false;
		}

		detail::std_function_job_vtable = memory::find_vtable_by_rtti (tier0, xs ("CStdFunctionJob"));
		if (!detail::std_function_job_vtable) {
			return false;
		}

		/*const auto api_set = GetModuleHandleA (xs ("api-ms-win-core-synch-l1-2-0.dll"));
		if (api_set) {
			detail::wait_on_address = reinterpret_cast<std::uintptr_t>(GetProcAddress (api_set, xs ("WaitOnAddress")));
		}*/

		return true;
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

	void parallel_for (int begin, int end, const std::function<void (int, int)>& body, int min_chunk_size, job_priority priority) {
		if (begin >= end) {
			return;
		}

		if (!detail::g_pool.valid () || !detail::g_mem_alloc || !detail::std_function_job_vtable) {
			body (begin, end);
			return;
		}

		const int total {end - begin};
		// Keep at most three queued chunks plus the caller; do not flood the
		// engine pool on machines with many logical CPUs.
		const int max_chunks {static_cast<int>(std::clamp (std::thread::hardware_concurrency (), 1u, 4u))};
		min_chunk_size = std::max (min_chunk_size, 1);

		auto chunk_size {(total + max_chunks - 1) / max_chunks};
		if (chunk_size < min_chunk_size) {
			chunk_size = min_chunk_size;
		}

		const auto num_chunks {(total + chunk_size - 1) / chunk_size};

		if (num_chunks <= 1) {
			body (begin, end);
			return;
		}

		std::exception_ptr failure;
		std::mutex failure_mutex;
		const auto invoke = [&] (int cb, int ce) {
			try {
				body (cb, ce);
			} catch (...) {
				std::lock_guard lock (failure_mutex);
				if (!failure) failure = std::current_exception ();
			}
		};

		std::vector<job> jobs;
		jobs.reserve (static_cast<std::size_t>(num_chunks) - 1);
		{
			// Also join on dispatch/allocation failure before references captured
			// by already queued jobs can leave scope.
			struct join_guard {
				std::vector<job>& jobs;
				~join_guard () { for (auto& j : jobs) j.wait (); }
			} guard {jobs};

			for (auto i = 0; i < num_chunks - 1; ++i) {
				const auto cb {begin + i * chunk_size};
				const auto ce {std::min (cb + chunk_size, end)};
				jobs.push_back (run ([&invoke, cb, ce] () { invoke (cb, ce); }, priority));
				if (!jobs.back ()) invoke (cb, ce);
			}

			invoke (begin + (num_chunks - 1) * chunk_size, end);
		}
		if (failure) std::rethrow_exception (failure);
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