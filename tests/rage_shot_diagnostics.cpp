#include <utilities/rage_shot_diagnostics.hpp>
#include <cassert>
#include <cstring>
#include <limits>
#include <type_traits>

namespace sd = utilities::rage_shot_diagnostics;
int main()
{
    static_assert(std::is_trivially_copyable_v<sd::snapshot>);
    sd::snapshot value{};
    assert(value.method == sd::acceptance::none && value.selected == -1);
    assert(!value.previous.present && !value.chosen.present && !value.next.present);
    assert(std::strcmp(sd::method_name(value.method), "unavailable") == 0);
    assert(std::strcmp(sd::method_name(sd::acceptance::contact), "contact") == 0);
    assert(std::strcmp(sd::method_name(sd::acceptance::geometry), "geometry") == 0);
    assert(std::strcmp(sd::relation(0.5f, value.chosen), "unknown") == 0);
    value.chosen = {true, 0.25f, 0.75f, 42, 3, 1, 2, 1};
    for (float f : {0.25f, 0.5f, 0.75f})
        assert(std::strcmp(sd::relation(f, value.chosen), "inside") == 0);
    assert(std::strcmp(sd::relation(0.2f, value.chosen), "before") == 0);
    assert(std::strcmp(sd::relation(0.8f, value.chosen), "after") == 0);
    assert(std::strcmp(sd::relation(std::numeric_limits<float>::quiet_NaN(), value.chosen), "unknown") == 0);
    const auto copy = value;
    value.chosen.damage = 0;
    assert(copy.chosen.damage == 42);
    value.chosen.enter = 1;
    assert(std::strcmp(sd::relation(0.5f, value.chosen), "unknown") == 0);

    sd::report_budget budget;
    assert(budget.take(0));
    assert(!budget.take(0) && !budget.take(999));
    assert(budget.take(1000));
    assert(!budget.take(999)); // Defensive clock rollback handling.
    assert(budget.observed == 5 && budget.emitted == 2 && budget.suppressed == 3);
    for (std::uint64_t i = 2; i < 32; ++i) assert(budget.take(i * 1000));
    assert(!budget.take(32000));
    assert(!budget.take(1000000));
    assert(budget.emitted == 32 && budget.suppressed == 5);
}
