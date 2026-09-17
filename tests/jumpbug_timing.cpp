#include <core/features/movement/jumpbug_timing.hpp>
#include <cassert>
#include <cmath>
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
    // Falling at different speeds: locate the middle of the available clearance
    // window, not a fixed timer or a uniform 128-sample grid point.
    for (const float speed : {200.0f, 1000.0f, 3500.0f})
    {
        constexpr float dt = 1.0f / 64.0f;
        constexpr float gravity = 800.0f;
        constexpr float target_time = 0.73123f;
        const float height = speed * dt * target_time + 0.5f * gravity * dt * dt * target_time * target_time + 1.0f;
        const auto clearance = [&](float time) {
            return height - speed * dt * time - 0.5f * gravity * dt * dt * time * time;
        };
        const auto when = find_contact_time([&](float time) -> std::optional<bool> {
            return clearance(time) <= 1.0f;
        });
        assert(when && std::fabs(*when - target_time) < 2.0f / 65536.0f);
        assert(clearance(*when) > 0.0f && clearance(*when) < 2.0f);
        assert(clearance(*when + event_gap) > 0.0f);
    }
}
