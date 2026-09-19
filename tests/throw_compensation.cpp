#include <core/features/misc/throw_compensation.hpp>
#include <cassert>
#include <cmath>
#include <limits>
#include <random>

int main() {
    using namespace features::misc::throw_compensation;
    const auto verify = [](vector target, vector inherited, float speed) {
        const auto answer = solve(target, inherited, speed);
        if (!answer) return false;
        const float norm = std::sqrt(target[0]*target[0] + target[1]*target[1] + target[2]*target[2]);
        float unit = 0.0f;
        for (int i = 0; i < 3; ++i) {
            unit += answer->direction[i] * answer->direction[i];
            const float actual = speed * answer->direction[i] + inherited[i];
            const float expected = target[i] / norm * answer->forward_speed;
            assert(std::abs(actual - expected) < 0.002f);
        }
        assert(std::abs(unit - 1.0f) < 0.00001f);
        assert(answer->forward_speed > 0.0f);
        return true;
    };
    assert(verify({1,0,0}, {0,0,0}, 675));
    assert(verify({1,0,0}, {0,312.5f,375}, 675)); // right strafe + jump
    assert(verify({1,0,0}, {0,-312.5f,375}, 675)); // left strafe + jump
    assert(verify({1,0,0}, {-312.5f,0,-375}, 675));
    assert(!solve({1,0,0}, {0,312.5f,375}, 202.5f)); // weak throw cannot cancel this velocity
    assert(!solve({1,0,0}, {-700,0,0}, 675));
    assert(!solve({0,0,0}, {0,0,0}, 675));
    assert(!solve({1,0,0}, {0,0,0}, 0));
    assert(!solve({NAN,0,0}, {0,0,0}, 675));
    assert(!solve({1,0,0}, {INFINITY,0,0}, 675));
    assert(!solve({1,0,0}, {0,0,0}, INFINITY));
    assert(!input_pitch(NAN));
    assert(!input_pitch(-90));
    assert(!input_pitch(90));
    for (int i = -890; i <= 890; ++i) {
        const float input = i * 0.1f;
        const float launch = input - (90.0f - std::abs(input)) / 9.0f;
        const auto inverse = input_pitch(launch);
        assert(inverse && std::abs(*inverse - input) < 0.0001f);
    }
    std::mt19937 rng(725);
    std::uniform_real_distribution<float> target(-1, 1), velocity(-400, 400);
    for (int i = 0; i < 10000; ++i)
        assert(verify({target(rng),target(rng),target(rng)}, {velocity(rng),velocity(rng),velocity(rng)}, 1000));
}
