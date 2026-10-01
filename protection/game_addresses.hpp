#ifndef GAME_ADDRESSES_HPP
#define GAME_ADDRESSES_HPP

#include <cstdint>
#include <cstddef>
#include <atomic>
#include <chrono>

/*
* @info
* All addresses have meta data like
* All patterns live in protection/patterns.hpp — update signatures there 1:1.
* PATTERN ( patterns::name )
* MODULE ( "MODULE" )
* INTERFACE_ ( "MODULE:INTERFACE_NAME" )
* CONVAR ( "CONVAR_NAME" )
*
* We dont need to store decrypted address, we need to use it in code
*
*
* for example:
*
* bad:
* detail::address_1 = PATTERN (patterns::name); then in another function call detail::address_1
* good:
* call (PATTERN (patterns::name))
*/

namespace protection::addresses {
	constexpr std::uint32_t hash_const (const char* str, std::uint32_t value = 0x811C9DC5u) {
		return *str ? hash_const (str + 1, (value ^ std::uint32_t (*str)) * 0x01000193u) : value;
	}

	// Backoff helper for the PATTERN() retry. A failed resolution doubles the
	// interval before the next attempt, capped at ~30 s. prev_due is the
	// previous deadline, now the current steady_clock tick.
	[[gnu::always_inline]] inline long long pattern_retry_backoff_ns (long long prev_due, long long now) noexcept {
		constexpr long long k_second = 1000000000LL;
		constexpr long long k_cap = 30LL * k_second;
		const long long elapsed = now - prev_due;
		long long delay = k_second;
		if ( prev_due && elapsed > 0 && elapsed < k_cap )
		{
			delay = elapsed * 2;
			if ( delay > k_cap ) delay = k_cap;
		}
		return delay;
	}

	template <std::size_t N>
	consteval std::uint32_t hash (const char (&str) [N]) {
		return hash_const (str);
	}

	template <std::size_t N>
	struct fixed_string {
		char value [N] {};

		constexpr fixed_string (const char (&str) [N]) {
			for (std::size_t i = 0; i < N; ++i)
				value [i] = str [i];
		}

		constexpr operator const char* () const {
			return value;
		}
	};

	template <std::size_t N>
	fixed_string (const char (&) [N]) -> fixed_string<N>;

	enum class address_type : std::uint32_t {
		pattern,
		interface_,
		convar,
		module_base,
		module_export
	};

	struct address_data_t {
		address_type type;
		const char* data;
	};

	union address_t {
		address_data_t data;
		struct {
			volatile std::uintptr_t encoded;
			volatile std::uint64_t  key;
		};

		constexpr address_t (std::uintptr_t addr = 0) : encoded (addr), key (0) {}
		constexpr address_t (address_type type, const char* str) : data {type, str} {}
	};

	__forceinline std::uintptr_t decode (const address_t& e) noexcept {
		return static_cast<std::uintptr_t>(e.encoded ^ e.key);
	}

	template <std::uint32_t Hash, address_type Type, fixed_string Str>
	struct address_holder {
		[[gnu::section("_addr"), gnu::used, gnu::retain]]
		inline static constexpr address_t entry {Type, Str.value};
	};

	struct sentinel_holder {
		[[gnu::section("_addr"), gnu::used, gnu::retain]]
		inline static constexpr address_t entry {};
	};

}

#define ADDRESS_IMPL(hash_value, addr_type, str_value) \
    (::protection::addresses ::address_holder<hash_value, addr_type, ::protection::addresses ::fixed_string{ str_value }>::entry)

#ifndef PATTERN
// Per-call-site cache that RETRIES a miss (the old `static const auto val`
// poisoned a 0 forever when the first call happened before the module was
// loaded or after a game update changed the bytes).
//
// A retry is rate limited: memory::resolve_pattern() SIMD-scans the whole
// module (~39 MB for client.dll). Re-scanning every frame for a genuinely
// dead signature produced multi-second frame stalls, so misses back off
// exponentially up to ~30 s. resolve_pattern_cached() only caches successes.
#define PATTERN(entry) \
    ([]() -> std::uintptr_t { \
        static std::atomic<std::uintptr_t> val{}; \
        auto cur = val.load( std::memory_order_acquire ); \
        if ( !cur ) { \
            static std::atomic<long long> next_try{}; \
            const auto now = std::chrono::steady_clock::now().time_since_epoch().count(); \
            auto due = next_try.load( std::memory_order_acquire ); \
            if ( now >= due ) { \
                cur = memory::resolve_pattern_cached((entry).data.data); \
                if ( cur ) { \
                    val.store( cur, std::memory_order_release ); \
                    next_try.store( 0, std::memory_order_relaxed ); \
                } else { \
                    /* Exponential backoff: 1s, 2s, 4s, ... capped at 30s. */ \
                    const auto prev = next_try.exchange( now + ::protection::addresses::pattern_retry_backoff_ns( due, now ), \
                        std::memory_order_acq_rel ); \
                    (void)prev; \
                } \
            } \
        } \
        return cur; \
    }())
#endif

#ifndef INTERFACE_
#define INTERFACE_(str) \
    []() -> std::uintptr_t { \
        static const auto val = memory::get_module_interface(str); \
        return val; \
    }()
#endif

#ifndef CONVAR
#define CONVAR(str) \
    []() -> c_convar* { \
        if ( !addresses::globals::cvar ) return nullptr; \
        static const auto val = addresses::globals::cvar->find(::protection::addresses::hash(str)); \
        return val; \
    }()
#endif

#ifndef MODULE_BASE
#define MODULE_BASE(str) \
    []() -> std::uintptr_t { \
        static const auto val = memory::get_module_base(str); \
        return val; \
    }()
#endif

#ifndef MODULE_EXPORT
#define MODULE_EXPORT(str) \
    ([]() -> std::uintptr_t { \
        static std::uintptr_t val{}; \
        if ( !val ) { \
            val = memory::get_module_export( str ); \
        } \
        return val; \
    }())
#endif

#include <protection/patterns.hpp>

#endif // !GAME_ADDRESSES_HPP