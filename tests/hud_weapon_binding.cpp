#include <utilities/hud_weapon_binding.hpp>
#include <array>
#include <cassert>

int main() {
    using hud_binding::candidate;
    using hud_binding::select;
    using hud_binding::status;
    constexpr std::uint32_t active = 0x100081;
    constexpr std::uint32_t recycled = 0x200081; // Same entity slot, different generation.
    std::array<candidate, 1> one{{{0x10000, active, true, true}}};
    assert(select(one, active, true).entity == one[0].entity);
    assert(select(one, active, false).state == status::scene_model);
    assert(!select({}, active, false).entity);
    assert(!select(one, 0, false).entity);
    assert(!select(one, hud_binding::invalid_handle, false).entity);

    one[0].weapon = recycled;
    assert(!select(one, active, true).entity); // No fallback for a known wrong binding.
    assert(select(one, active, false).entity == one[0].entity); // Missing schema reads no handle.
    one[0].weapon = hud_binding::invalid_handle;
    assert(!select(one, active, true).entity);
    assert(select(one, active, false).entity == one[0].entity);

    one[0].model_matches = false;
    assert(select(one, active, false).state == status::model_mismatch);
    one[0].weapon = active;
    assert(select(one, active, true).entity == one[0].entity); // Linked knives can change models.
    one[0].model_matches = true;
    one[0].ready = false;
    assert(!select(one, active, true).entity);
    assert(!select(one, active, false).entity);
    one[0].ready = true;
    one[0].entity = 0;
    assert(!select(one, active, false).entity);

    std::array<candidate, 2> switching{{
        {0x10000, recycled, true, false},
        {0x20000, active, true, true}
    }};
    assert(select(switching, active, false).state == status::ambiguous);
    assert(select(switching, active, true).entity == switching[1].entity);
    switching[0].model_matches = true; // Switching between two guns of the same type.
    assert(!select(switching, active, false).entity);
    switching[0].ready = false; // Do not ignore an unready old child for uniqueness.
    assert(!select(switching, active, false).entity);
    switching[0].ready = true;
    switching[0].weapon = active; // Duplicate links are ambiguous too.
    assert(select(switching, active, true).state == status::ambiguous);
}
