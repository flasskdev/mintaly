#include <utilities/cosmetic_identity.hpp>
#include <cassert>

int main() {
    using cosmetic_cache::identity;
    using cosmetic_cache::reusable;
    const identity original{0x10000, 0x20000, 0x30000, 17};
    assert(reusable(original, original));
    assert(!reusable({}, {}));
    assert(!cosmetic_cache::hud_reusable({}, {}, true));
    assert(cosmetic_cache::hud_reusable({}, {}, false));
    assert(cosmetic_cache::hud_reusable(original, original, true));
    auto pending_hud = original;
    pending_hud.model = 0;
    assert(!cosmetic_cache::hud_reusable(pending_hud, pending_hud, true));
    assert(!cosmetic_cache::hud_reusable(pending_hud, original, true));
    auto changed = original;
    changed.owner = 18; // Pickup by another pawn needs a rebuild even with the same skin.
    assert(!reusable(original, changed));
    changed = original;
    changed.scene = 0x40000; // Scene recreation while metadata is unchanged.
    assert(!reusable(original, changed));
    changed = original;
    changed.model = 0x50000;
    assert(!reusable(original, changed));
    changed = original;
    changed.weapon = 0x60000;
    assert(!reusable(original, changed));
    changed = original;
    changed.model = 0; // Not ready: never accept the cached application.
    assert(!reusable(original, changed));
    changed = original;
    changed.scene = 0;
    assert(!reusable(original, changed));
}
