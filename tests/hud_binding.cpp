#include <utilities/hud_binding.hpp>

using hud_binding::matches;
// Compile-time tests remain active even in release builds.
static_assert(matches(0x10001u, 0x20000u, 0x10001u, 0, 0));
static_assert(matches(0x10001u, 0x20000u, std::nullopt, 0x10001u, 0));
static_assert(matches(0x10001u, 0x20000u, std::nullopt, 0, 0x20000u));
static_assert(!matches(0x10001u, 0x20000u, std::nullopt, 0, 0));
static_assert(!matches(0x10001u, 0x20000u, std::nullopt, 42, 0));
static_assert(!matches(0x10001u, 0x20000u, 0x20001u, 0x10001u, 0));
static_assert(!matches(0x10001u, 0x20000u, 0xffffffffu, 0, 0x20000u));
static_assert(!matches(0x10001u, 0x20000u, 0x10001u, 0, 0x30000u));
static_assert(!matches(0x10001u, 0, 0x10001u, 0, 0));
static_assert(!matches(0, 0x20000u, 0, 0, 0));
static_assert(!matches(0xffffffffu, 0x20000u, 0xffffffffu, 0, 0));
int main() {}
