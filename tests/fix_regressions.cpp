// Standalone helper tests. These do not exercise the CS2 engine or its ABI.
#include <utilities/event_key.hpp>
#include <utilities/cosmetic_model.hpp>
#include <utilities/lobby_music_queue.hpp>
#include <cassert>
#include <string>

static_assert(event_key::hash(nullptr) == 0);
static_assert(event_key::hash("") == 0xb5d89f2fu);
static_assert(event_key::hash("a") == 0x1ecf71e1u);
static_assert(event_key::hash("ab") == 0xe7ff0c8au);
static_assert(event_key::hash("abc") == 0xd6cf0d16u);
static_assert(event_key::hash("abcd") == 0xb42825ecu);
static_assert(event_key::hash("attacker") == 0xc819dd8au);
static_assert(event_key::hash("userid") == 0x3e7b804bu);
static_assert(event_key::hash("dmg_health") == 0x6cdfdf9du);
static_assert(event_key::hash("hitgroup") == 0x103e09f1u);

int main() {
    using event_key::local_attacker;
    assert(local_attacker(100, 100, 100, 200)); // Local hits enemy.
    assert(!local_attacker(100, 100, 200, 300)); // Enemy kills enemy teammate.
    assert(!local_attacker(100, 100, 200, 100)); // Local takes damage.
    assert(!local_attacker(100, 100, 0, 200));   // World / missing attacker.
    assert(!local_attacker(100, 100, 100, 100)); // Self damage.
    assert(!local_attacker(100, 100, 100, 0));   // Missing victim.
    assert(!local_attacker(100, 200, 100, 300)); // Stale snapshot.
    assert(!local_attacker(0, 0, 0, 200));       // Disconnected.

    assert(cosmetic_model::matches("Characters\\Test.VMDL_C", "characters/test.vmdl"));
    assert(cosmetic_model::matches("weapons/knife.vmdl", "weapons/knife.vmdl_c"));
    assert(!cosmetic_model::matches("", ""));
    assert(!cosmetic_model::matches("weapons/old.vmdl", "weapons/new.vmdl"));
    assert(cosmetic_model::canonical("test.vmdl_extra") == "test.vmdl_extra");

    lobby_music::queue q;
    assert(!q.next());
    q.submit({3, "first"});
    const auto first = q.next();
    assert(first && first->value.kit == 3);
    // Failed engine attempts do not acknowledge a request.
    assert(q.next()->generation == first->generation);
    q.submit({3, "first"});
    assert(q.next()->generation == first->generation);
    q.submit({4, "second"});
    const auto second = q.next();
    assert(second && second->generation != first->generation);
    q.acknowledge(*first); // A late completion must not erase the newer edit.
    assert(q.next()->value.kit == 4);
    q.acknowledge(*second);
    assert(!q.next());
    q.submit({4, "second"});
    assert(!q.next()); // Do not restart music each frame.
    q.submit({0, ""});
    const auto clear = q.next();
    assert(clear && clear->value.kit == 0);
    q.acknowledge(*clear);
    q.submit({0, ""});
    assert(!q.next());
    q.reset();
    q.submit({4, "second"});
    const auto reentry = q.next();
    assert(reentry && reentry->generation != second->generation);
    q.acknowledge(*second);
    assert(q.next()); // Old map completion cannot consume a new request.
    q.acknowledge(*reentry);
    assert(!q.next());
}
