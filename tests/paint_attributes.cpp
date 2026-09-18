#include <utilities/paint_attributes.hpp>
#include <cassert>

int main() {
    const cosmetic_paint::values expected{10006.0f, 42.0f, 0.01f};
    cosmetic_paint::snapshot attributes{};
    int writes = 0;
    bool readable = true;
    bool accept_writes = true;
    const auto read = [&](cosmetic_paint::snapshot& out) {
        out = attributes;
        return readable;
    };
    const auto set = [&](std::size_t slot, float value) {
        ++writes;
        if (accept_writes) attributes[slot] = {value, true};
    };

    // Empty engine vector: create all three, then verify using a fresh read.
    assert(cosmetic_paint::ensure(expected, read, set));
    assert(writes == 3);
    assert(cosmetic_paint::matches(attributes, expected));
    assert(cosmetic_paint::ensure(expected, read, set));
    assert(writes == 3);

    attributes[1] = {};
    attributes[2].value = 0.5f;
    writes = 0;
    assert(cosmetic_paint::ensure(expected, read, set));
    assert(writes == 2); // Partial initialization and changed wear.

    attributes = {}; // Respawn recreates the item view.
    accept_writes = false;
    assert(!cosmetic_paint::ensure(expected, read, set));
    accept_writes = true;
    assert(cosmetic_paint::ensure(expected, read, set));

    attributes = {};
    readable = false;
    writes = 0;
    assert(!cosmetic_paint::ensure(expected, read, set));
    assert(writes == 0);
    readable = true;
    const auto partial_set = [&](std::size_t slot, float value) {
        if (slot != 1) attributes[slot] = {value, true};
    };
    assert(!cosmetic_paint::ensure(expected, read, partial_set));
    assert(cosmetic_paint::ensure(expected, read, set));
    const auto unreadable_after_set = [&](std::size_t slot, float value) {
        set(slot, value);
        readable = false;
    };
    attributes = {};
    assert(!cosmetic_paint::ensure(expected, read, unreadable_after_set));
}
