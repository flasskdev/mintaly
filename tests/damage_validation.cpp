#include <core/features/combat/damage_validation.hpp>
#include <array>
#include <cassert>
#include <limits>

namespace dv = features::combat::damage_validation;
int main()
{
    const auto nan = std::numeric_limits<float>::quiet_NaN();
    const auto inf = std::numeric_limits<float>::infinity();
    // Reproduce the failed-open comparison from the uploaded shot log.
    assert(!(nan < 34.0f));
    assert(!dv::meets_minimum(nan, 34.0f));
    const std::array invalid{nan, -nan, inf, -inf, 0.0f, -0.0f, -1.0f};
    for (const float damage : invalid) {
        assert(!dv::positive_finite(damage));
        assert(!dv::meets_minimum(damage, 0.0f));
        assert(!dv::meets_minimum(damage, 34.0f));
    }
    for (const float minimum : std::array{nan, -nan, inf, -inf, -1.0f})
        assert(!dv::meets_minimum(100.0f, minimum));
    assert(dv::meets_minimum(34.0f, 34.0f));
    assert(dv::meets_minimum(35.0f, 34.0f));
    assert(!dv::meets_minimum(33.0f, 34.0f));
    assert(dv::meets_minimum(1.0f, 0.0f));
    assert(dv::positive_finite(std::numeric_limits<float>::max()));
    assert(dv::meets_minimum(std::numeric_limits<float>::max(), std::numeric_limits<float>::max()));

    // Finite trace damage does not guarantee a usable scaled result.
    const float raw = 1.0452f;
    const float scaled = std::floor(raw * nan);
    assert(!dv::positive_finite(scaled));
    assert(!dv::meets_minimum(scaled, 34.0f));
    assert(!dv::meets_minimum(std::floor(raw * 0.0f), 0.0f));
    assert(dv::meets_minimum(std::floor(20.0f * 2.0f), 34.0f));
}
