#include "../core/features/movement/air_acceleration.hpp"
#include <cstdlib>
#include <iostream>

namespace {
    void require(bool condition)
    {
        if (!condition)
        {
            std::cerr << "Air acceleration regression failed\n";
            std::exit(EXIT_FAILURE);
        }
    }

    bool close(float a, float b, float epsilon = 0.002f)
    {
        return std::fabs(a - b) <= epsilon;
    }
}

int main()
{
    using features::movement::detail::ideal_air_angle;
    using features::movement::detail::simulate_air_acceleration;

    require(close(ideal_air_angle(0.0f, 0.01f, 250.0f, 12.0f, 1.0f, 30.0f), 0.0f));
    require(close(ideal_air_angle(300.0f, 0.02f, 250.0f, 12.0f, 1.0f, 30.0f), 90.0f));

    // Friction and the cap must agree between angle selection and simulation.
    for (const auto friction : { 0.25f, 0.5f, 1.0f })
    {
        for (const auto dt : { 0.001f, 0.008f, 0.016f })
        {
            for (const auto speed : { 5.0f, 100.0f, 300.0f, 1000.0f })
            {
                const auto angle = ideal_air_angle(speed, dt, 250.0f, 12.0f, friction, 30.0f);
                auto vx = speed;
                auto vy = 0.0f;
                simulate_air_acceleration(vx, vy, angle, dt, 250.0f, 12.0f, friction, 30.0f);
                const auto best = vx * vx + vy * vy;
                // Compare with a numerical sweep of all possible wish angles.
                for (int degrees = 0; degrees <= 180; ++degrees)
                {
                    auto x = speed;
                    auto y = 0.0f;
                    simulate_air_acceleration(x, y, static_cast<float>(degrees), dt,
                        250.0f, 12.0f, friction, 30.0f);
                    require(x * x + y * y <= best + std::max(0.05f, best * 0.000002f));
                }
            }
        }
    }

    // No fictitious 15% bonus; acceleration is capped by available wishspeed.
    float vx = 0.0f, vy = 0.0f;
    simulate_air_acceleration(vx, vy, 0.0f, 0.001f, 250.0f, 12.0f, 0.5f, 30.0f);
    require(close(vx, 1.5f) && close(vy, 0.0f));
    vx = 29.0f;
    simulate_air_acceleration(vx, vy, 0.0f, 0.02f, 250.0f, 12.0f, 1.0f, 30.0f);
    require(close(vx, 30.0f));
    vx = 100.0f;
    simulate_air_acceleration(vx, vy, 0.0f, 0.02f, 250.0f, 12.0f, 1.0f, 30.0f);
    require(close(vx, 100.0f));

    // World-space acceleration is independent of the command/anti-aim basis.
    for (const auto yaw : { -179.0f, -90.0f, 0.0f, 45.0f, 179.0f })
    {
        const auto radians = yaw * (std::numbers::pi_v<float> / 180.0f);
        float x = 300.0f, y = 0.0f;
        float rotated_x = x * std::cos(radians), rotated_y = x * std::sin(radians);
        const auto angle = ideal_air_angle(300.0f, 0.001f, 250.0f, 12.0f, 1.0f, 30.0f);
        simulate_air_acceleration(x, y, angle, 0.001f, 250.0f, 12.0f, 1.0f, 30.0f);
        simulate_air_acceleration(rotated_x, rotated_y, angle + yaw, 0.001f, 250.0f, 12.0f, 1.0f, 30.0f);
        require(close(std::hypot(x, y), std::hypot(rotated_x, rotated_y)));
    }
}
