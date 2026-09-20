#include <core/features/movement/jumpbug_cycle.hpp>
#include <cassert>
#include <limits>

int main() {
    features::movement::jumpbug_cycle cycle;
    assert(cycle.available(false, -700));
    cycle.fired();
    assert(!cycle.available(true, 0));
    assert(!cycle.available(false, 300));
    assert(!cycle.available(false, 0));
    assert(cycle.available(false, -700)); // descent after hop can need another jumpbug
    cycle.fired();
    assert(cycle.available(false, -800)); // missed/unconfirmed jump must not lock the fall
    cycle.fired();
    assert(!cycle.available(true, 250));
    assert(!cycle.available(true, 0));
    assert(cycle.available(true, 0));
    assert(cycle.available(false, -700));
    cycle.fired();
    assert(!cycle.available(false, std::numeric_limits<float>::quiet_NaN()));
    cycle = {};
    assert(cycle.available(false, -700));
}
