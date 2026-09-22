"""Source-wiring regressions only: not a C++ compilation or CS2 runtime test."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]


def source(path):
    return (ROOT / path).read_text(encoding="utf-8")


def between(text, start, end):
    return text.split(start, 1)[1].split(end, 1)[0]


class SkinApplicationChecks(unittest.TestCase):
    def test_missing_hud_does_not_block_item_writes(self):
        guns = source("core/features/changer/impl/guns.cpp")
        apply = between(guns, "bool guns::apply(", "bool guns::capture_original(")
        self.assertNotIn("find_hud_model_weapon", apply)
        self.assertIn("cosmetic_attributes::apply( iv, *skin )", apply)
        self.assertLess(apply.index("this->rebuild_paint"), apply.index("original->second.cosmetic ="))

    def test_world_rebuild_precedes_optional_hud_work(self):
        guns = source("core/features/changer/impl/guns.cpp")
        rebuild = between(guns, "bool guns::rebuild_paint(", "bool guns::update_view_model(")
        self.assertLess(rebuild.index("entity_guard::rebuild_materials"), rebuild.index("this->update_view_model"))
        hud_work = rebuild.split("if ( needs_hud", 1)[1]
        self.assertNotIn("return false", hud_work)
        self.assertIn('report_skin_failure( "hud-pending"', hud_work)
        self.assertIn("entity_guard::current( *entity )", hud_work)

    def test_hud_retry_only_completes_after_success(self):
        guns = source("core/features/changer/impl/guns.cpp")
        render = between(guns, "void guns::on_render_start(", "std::uintptr_t guns::find_hud_model_weapon(")
        failure = between(render, "if (!this->update_view_model", "// Engine callbacks")
        self.assertIn("return;", failure)
        self.assertNotIn("hud_refresh_pending = false", failure)
        self.assertIn("current->second.hud_refresh_pending = false", render)
        self.assertIn("local.observer_pawn", render)
        self.assertIn("bool hud_refresh_pending{true}", source("core/features/changer/changer.hpp"))

    def test_hud_lookup_resolves_inherited_field_without_guessing_handles(self):
        hud = source("core/features/changer/hud_weapon.hpp")
        resolver = between(hud, "inline std::uint32_t weapon_handle_offset()", "// Multiple HUD")
        self.assertIn('SCHEMA("C_CS2HudModelWeapon", "m_hWeapon"_hash)', resolver)
        self.assertIn('SCHEMA("C_CS2HudModelBase", "m_hWeapon"_hash)', resolver)
        self.assertIn("const auto weapon_offset = weapon_handle_offset();", hud)
        self.assertIn("hud_binding::select(", hud)
        self.assertIn("i < 128", hud)
        locate = between(hud, "inline lookup_result locate(", "inline std::uintptr_t find(")
        self.assertNotIn("|| !weapon_offset", locate)
        self.assertIn("!weapon_offset && !forward && count == 1", locate)
        self.assertIn("candidates[0].model_matches", locate)
        self.assertIn("owned_by_player()", locate)
        self.assertIn('"child-limit-or-cycle"', locate)
        self.assertIn("memory::safe_read<std::uint32_t>(services + active_offset).value_or(0) != active", locate)
        self.assertIn("return locate(pawn).entity;", hud)
        self.assertIn("const bool should_bind = model_mismatch && is_firstperson;", hud)
        self.assertIn("hud_binding::matches(active, owner, reverse, candidate.owner, forward)", hud)
        self.assertIn('SCHEMA("C_BaseEntity", "m_hOwnerEntity"_hash)', hud)
        self.assertIn('SCHEMA("C_CSWeaponBase", "m_hHudModel"_hash)', hud)
        self.assertIn('if (!reverse) return {0, "unreadable-reverse-link", count};', hud)
        self.assertIn('if (has_forward && !forward) return {0, "unready-forward-link"};', hud)

    def test_ambiguous_children_do_not_trigger_rebinding(self):
        hud = source("core/features/changer/hud_weapon.hpp")
        update = hud.split("inline bool update(", 1)[1]
        self.assertLess(update.index("if (!existing_hud && lookup.candidates) return false;"),
                        update.index("memory::call<void>(binding, weapon->entity)"))

    def test_hud_files_have_no_merge_markers(self):
        for path in ("core/features/changer/hud_weapon.hpp", "tests/skin_application_checks.py"):
            for line in source(path).splitlines():
                self.assertFalse(line.startswith(("<<<<<<<", "=======", ">>>>>>>")), path)

    def test_pickup_source_precedes_holder_loadout(self):
        guns = source("core/features/changer/impl/guns.cpp")
        select = between(guns, "std::optional<guns::skin_selection> guns::select_skin(",
                         "void guns::on_frame_stage_notify(")
        self.assertLess(select.index("return original.cosmetic;"), select.index("const auto selected = skins.find"))
        self.assertIn('SCHEMA( "C_EconEntity", "m_OriginalOwnerXuidLow"_hash )', select)
        self.assertIn("g_skin_sync.get_remote_skin( source_sid )", select)
        donor = between(select, "if ( source_account && source_account != holder_account )",
                        "const auto selected = skins.find")
        self.assertIn("if ( !profile ) return std::nullopt;", donor)
        self.assertIn("if ( selected == profile->skins.end( ) ) return std::nullopt;", donor)

    def test_missing_binding_has_bounded_schema_diagnostics(self):
        hud = source("core/features/changer/hud_weapon.hpp")
        self.assertIn("std::chrono::seconds(5)", hud)
        self.assertIn("systems::schemas::dump_fields(name)", hud)
        schema = source("core/systems/impl/schemas.cpp")
        self.assertIn("*count > 256", schema)
        self.assertIn("memory::read_string( name, 128 )", schema)

    def test_attribute_readback_uses_runtime_schema(self):
        attrs = source("core/features/changer/cosmetic_attributes.hpp")
        capture = between(attrs, "inline bool capture(", "inline bool matches(")
        for field in ("m_AttributeList", "m_Attributes", "m_iAttributeDefinitionIndex", "m_flValue"):
            self.assertIn(field, capture)
        for old_offset in ("item_view + 0x210", "item_view + 0x218", "attribute + 0x30", "attribute + 0x34"):
            self.assertNotIn(old_offset, capture)
        self.assertIn("*count > 16384", capture)
        self.assertIn("(*count && !*data)", capture)
        self.assertIn("actual == saved", attrs)
        self.assertIn("return matches(item_view, skin)", attrs)

    def test_failures_have_rate_limited_diagnostics(self):
        guns = source("core/features/changer/impl/guns.cpp")
        self.assertIn("std::chrono::seconds( 5 )", guns)
        for reason in ("attribute-dependencies", "original-snapshot", "attribute-readback", "world-material-rebuild", "hud-pending"):
            self.assertIn('"' + reason + '"', guns)
        self.assertIn("[skin-apply] reason=", guns)


if __name__ == "__main__":
    unittest.main(verbosity=2)
