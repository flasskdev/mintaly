#include <core/features/movement/jumpbug_cycle.hpp>
#include <cassert>

int main() {
    features::movement::jumpbug_cycle cycle;
    assert(cycle.available(false, -700));
    cycle.fired();
    assert(!cycle.available(true, 0)); // one transient ground sample cannot re-arm
    assert(!cycle.available(false, 300));
    for (int i = 0; i < 100; ++i) assert(!cycle.available(false, -700));
    assert(!cycle.available(true, 250)); // ground bit during synthetic jump
    assert(!cycle.available(true, 0));
    assert(cycle.available(true, 0)); // settled on the ground
    assert(cycle.available(false, -700)); // next independent fall
    cycle.fired();
    assert(!cycle.available(false, -700));
    cycle = {};
    assert(cycle.available(false, -700)); // pawn reset
}
