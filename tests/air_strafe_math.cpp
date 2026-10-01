#include <core/features/movement/air_strafe_math.hpp>
#include <cassert>
#include <cmath>
#include <initializer_list>
#include <numbers>

int main() {
    using namespace features::movement::air_strafe_math;
    constexpr float radians = std::numbers::pi_v<float> / 180.0f;
    assert(normalize(540.0f) == -180.0f);
    assert(wish_yaw(0.0f, 0.0f, 90.0f, 30.0f, 6.0f, true) == 90.0f);
    for (float speed : {0.0f, 10.0f, 100.0f, 250.0f, 390.0f, 420.0f})
    for (float direction : {-180.0f, -135.0f, -90.0f, -45.0f, 0.0f, 45.0f, 90.0f, 135.0f, 180.0f})
    for (float accel : {0.1f, 6.0f, 30.0f, 60.0f}) {
        const float yaw = wish_yaw(speed, 0.0f, direction, 30.0f, accel, true);
        assert(std::isfinite(yaw));
        const float x = std::cos(yaw * radians), y = std::sin(yaw * radians);
        const float gain = std::clamp(30.0f - speed * x, 0.0f, accel);
        const float result = std::hypot(speed + gain * x, gain * y);
        assert(result + 0.001f >= speed);
        if (speed > 30.0f) {
            assert(result > speed);
            assert(direction == 0.0f || std::fabs(direction) == 180.0f || (direction > 0.0f ? y > 0.0f : y < 0.0f));
        }
        const float relative = (yaw - 37.0f) * radians;
        const float f = std::cos(relative), l = std::sin(relative);
        assert(std::fabs(f * f + l * l - 1.0f) < 0.00001f);
    }
    assert(wish_yaw(250.0f, 0.0f, 0.0f, 30.0f, 6.0f, true) > 0.0f);
    assert(wish_yaw(250.0f, 0.0f, 0.0f, 30.0f, 6.0f, false) < 0.0f);
}
