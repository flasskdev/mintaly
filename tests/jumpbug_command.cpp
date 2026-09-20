#include <core/features/movement/jumpbug_command.hpp>
#include <cassert>
#include <limits>

int main() {
    using namespace features::movement;
    constexpr std::uint64_t jump = 2, duck = 4, attack = 1;
    const auto prepare = jumpbug_command::make(attack | jump, jump, duck, std::nullopt, true);
    assert(prepare && prepare->count == 2);
    assert(prepare->final_buttons == (attack | duck));
    assert(prepare->owns_duck && !prepare->owns_jump);
    for (bool include_jump : {false, true}) {
        for (float release : {0.0f, 0.5f, jumpbug_timing::latest_release}) {
            const auto plan = jumpbug_command::make(attack | duck, jump, duck, release, include_jump);
            assert(plan);
            assert(!plan->owns_duck && plan->owns_jump == include_jump);
            assert(plan->final_buttons == (attack | (include_jump ? jump : 0)));
            auto replay = attack | duck;
            float previous = 0.0f;
            int duck_presses = 0;
            for (int i = 0; i < plan->count; ++i) {
                const auto& event = plan->events[i];
                assert(event.when >= previous && event.when < 1.0f);
                previous = event.when;
                if (event.pressed) replay |= event.button;
                else replay &= ~event.button;
                if (event.button == duck && event.pressed) ++duck_presses;
            }
            assert(replay == plan->final_buttons);
            assert(duck_presses == (release > 0.0f ? 1 : 0));
        }
    }
    for (float invalid : {-1.0f, 1.0f, std::numeric_limits<float>::infinity(),
        std::numeric_limits<float>::quiet_NaN()})
        assert(!jumpbug_command::make(0, jump, duck, invalid, true));
}
