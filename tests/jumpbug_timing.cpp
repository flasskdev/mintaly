#include <core/features/movement/jumpbug_timing.hpp>
#include <cassert>
#include <cmath>
#include <initializer_list>
#include <limits>
#include <optional>

int main()
{
    using namespace features::movement::jumpbug_timing;
    for (float crouched : {36.0f, 54.0f, 72.0f}) {
        const auto expansion = airborne_unduck_expansion(crouched, 72.0f);
        assert(expansion);
        const float lower = -*expansion;
        const float upper = crouched + *expansion;
        assert(upper - lower == 72.0f);
        assert((upper + lower) * 0.5f == crouched * 0.5f);
    }
    assert(airborne_unduck_expansion(54.0f, 72.0f) == 9.0f);
    assert(!airborne_unduck_expansion(0.0f, 72.0f));
    assert(!airborne_unduck_expansion(73.0f, 72.0f));
    assert(!airborne_unduck_expansion(NAN, 72.0f));
    assert(!airborne_unduck_expansion(54.0f, INFINITY));
    for (int i = 0; i <= 1000; ++i)
    {
        const float target = latest_release * static_cast<float>(i) / 1000.0f;
        int calls = 0;
        const auto when = find_contact_time([&](float time) -> std::optional<bool> {
            ++calls;
            return time >= target;
        });
        assert(when && *when >= target && *when - target <= 1.0f / 65536.0f);
        assert(*when + event_gap < 1.0f && *when + event_gap > *when);
        assert(calls <= 18);
    }
    assert(!find_contact_time([](float) -> std::optional<bool> { return false; }));
    assert(!find_contact_time([](float) -> std::optional<bool> { return std::nullopt; }));
    assert(!find_contact_time([](float time) -> std::optional<bool> {
        if (time > 0.4f && time < 0.6f) return std::nullopt;
        return time > 0.7f;
    }));

    constexpr float dt = 1.0f / 64.0f;
    assert(movement_velocity_z(-1000.0f, 800.0f, dt) == -1006.25f);
    assert(movement_velocity_z(-1000.0f, 0.0f, dt) == -1000.0f);
    assert(!movement_velocity_z(NAN, 800.0f, dt));
    assert(!movement_velocity_z(-1000.0f, INFINITY, dt));
    assert(!movement_velocity_z(-1000.0f, -1.0f, dt));
    assert(!movement_velocity_z(-1000.0f, 800.0f, 0.0f));
    assert(!movement_velocity_z(-1000.0f, 800.0f, NAN));
    assert(!movement_velocity_z(-std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), 4.0f));

    // Match the movement predictor at release AND jump, across gravity settings
    // and dangerous fall speeds. The previous parabolic path agreed only at t=1.
    for (const float gravity : {0.0f, 800.0f, 1600.0f})
    for (const float speed : {200.0f, 1000.0f, 3500.0f})
    {
        constexpr float target_time = 0.73123f;
        const auto velocity = movement_velocity_z(-speed, gravity, dt);
        assert(velocity);
        const float height = -*velocity * dt * target_time + 1.0f;
        const auto clearance = [&](float time) { return height + *velocity * dt * time; };
        const auto when = find_contact_time([&](float time) -> std::optional<bool> {
            return clearance(time) <= 1.0f;
        });
        assert(when && std::fabs(*when - target_time) < 2.0f / 65536.0f);
        assert(clearance(*when) > 0.0f && clearance(*when) < 2.0f);
        assert(clearance(*when + event_gap) > 0.0f);
    }

    // A thin platform can be crossed entirely inside the tick. The old endpoint
    // predicate misses it; a swept prefix still brackets its first contact.
    constexpr float enter = 0.40123f;
    constexpr float leave = enter + 2.0f * event_gap;
    const auto support_at = [](float time) -> std::optional<bool> {
        return time >= enter && time <= leave;
    };
    assert(!find_contact_time(support_at));
    const auto when = find_contact_time([](float time) -> std::optional<bool> {
        return time >= enter; // Prefix sweep intersects the platform.
    });
    assert(when && *support_at(*when) && *support_at(*when + event_gap));
    assert(*when - enter <= 1.0f / 65536.0f);

    // Already in the window must release immediately; no duplicate crouch press.
    const auto immediate = find_contact_time([](float) -> std::optional<bool> { return true; });
    assert(immediate && *immediate == 0.0f);

    // Inside the expanded probe, but actual standing hull is still clear and
    // supported: release now rather than rejecting an all-solid probe forever.
    int probes = 0;
    const auto already_supported = find_release_time([&](float) -> std::optional<bool> {
        ++probes;
        return std::nullopt;
    }, [](float t) { return t == 0.0f; });
    assert(already_supported && *already_supported == 0.0f && probes == 0);
    assert(!find_release_time([](float) -> std::optional<bool> { return std::nullopt; },
        [](float) { return false; }));
    assert(!find_release_time([](float t) -> std::optional<bool> { return t >= 0.5f; },
        [](float) { return false; })); // blocked head / unsupported landing
    const auto later = find_release_time([](float t) -> std::optional<bool> { return t >= 0.5f; },
        [](float t) { return t >= 0.5f && t < 0.51f; });
    assert(later && *later >= 0.5f && *later < 0.51f);
}
