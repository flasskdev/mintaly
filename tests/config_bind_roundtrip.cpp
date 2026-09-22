// Windows integration test: link with the project's xui/config dependencies.
// This test is provided for the native build; it is not a standalone Linux target.
#include <core/settings.hpp>
#include <cassert>
#include <unordered_set>

namespace {
    void exercise(settings::combat::ragebot::weapon_group& group, int seed) {
        group.max_fov.value = 90.0f + seed;
        group.hitchance.value = 30 + seed;
        group.min_damage.value = 40 + seed;
        group.min_damage_override_value.value = 20 + seed;
        group.hitchance_override_value.value = 50 + seed;
        group.pointscale.value = 60.0f + seed;
        group.hitboxes[seed % 6] = true;
        for (auto* setting : {&group.silent, &group.no_spread, &group.body_aim,
             &group.force_shot_air, &group.force_shot, &group.autostop,
             &group.min_damage_override, &group.hitchance_override,
             &group.dynamic_pointscale, &group.debug_multipoints}) {
            setting->value = seed % 2 == 0;
            setting->bind.key = 'J';
            setting->bind.mode = xui::bind_mode::toggle;
            setting->bind.active = setting->value;
        }
        for (void* ptr : {static_cast<void*>(&group.max_fov.value),
             static_cast<void*>(&group.hitchance.value), static_cast<void*>(&group.min_damage.value),
             static_cast<void*>(&group.min_damage_override_value.value),
             static_cast<void*>(&group.hitchance_override_value.value), static_cast<void*>(&group.pointscale.value)}) {
            auto* entry = xui::slider_binds::find_by_ptr(ptr);
            assert(entry && entry->config_key); // Available before drawing any tab.
            entry->count = 1;
            entry->binds[0] = {'H', xui::bind_mode::hold_on, 12.0f, false};
        }
    }
}

int main() {
    config::initialize();
    const auto field_count = config::detail::get_registry().fields.size();
    config::initialize();
    assert(config::detail::get_registry().fields.size() == field_count);
    std::unordered_set<std::uint32_t> keys;
    for (const auto& field : config::detail::get_registry().fields)
        assert(keys.insert(field.key).second); // Detect missing per-weapon category isolation.

    auto& rage = settings::g_combat.m_ragebot;
    int seed = 0;
    for (auto& group : rage.groups) exercise(group, ++seed);
    for (auto& weapon : rage.weapons) {
        exercise(weapon.cfg, ++seed);
        weapon.override_group.value = seed % 2 == 0;
    }
    const auto saved = config::to_json();
    auto& example = rage.groups[0].hitchance;
    const auto base_value = example.value;
    xui::input_state input{};
    xui::slider_binds::process(input);
    input.keys['H'] = true;
    xui::slider_binds::process(input);
    assert(example.value == 12);
    assert(config::to_json() == saved); // Save while held preserves the base, not 12.

    assert(config::from_json({{"version", config::k_version}, {"fields", nlohmann::json::object()}}));
    assert(xui::slider_binds::serialize().empty());
    assert(example.value == 80); // No restoration of previous profile's base.
    xui::slider_binds::process(input);
    assert(example.value == 80);
    assert(config::from_json(saved));
    assert(example.value == base_value);
    assert(config::to_json() == saved);

    const auto delta = config::to_json_delta();
    assert(config::from_json_delta({{"v", config::k_version}, {"f", nlohmann::json::object()}}));
    assert(xui::slider_binds::serialize().empty());
    assert(config::from_json_delta(delta));
    assert(config::to_json() == saved);

    auto empty_binds = saved;
    empty_binds["sb"] = "[]";
    assert(config::from_json(empty_binds));
    assert(xui::slider_binds::serialize().empty());

    assert(config::from_json(saved));
    auto subset = nlohmann::json::parse(saved["sb"].get<std::string>());
    subset.erase(subset.begin() + 1, subset.end());
    xui::slider_binds::deserialize(subset.dump());
    assert(nlohmann::json::parse(xui::slider_binds::serialize()).size() == 1);
    xui::slider_binds::deserialize("not json");
    assert(xui::slider_binds::serialize().empty());

    auto* entry = xui::slider_binds::find_by_ptr(&example.value);
    const auto canonical_id = entry->id;
    assert(xui::slider_binds::get_or_create(&example.value, 123, "old label", 0, 100, true, "%d") == entry);
    assert(xui::slider_binds::get_or_create(&example.value, 456, "new label", 0, 100, true, "%d") == entry);
    assert(entry->id == canonical_id);
    auto& other = rage.groups[1].hitchance;
    assert(xui::slider_binds::get_or_create(&other.value, 456, "same UI id", 0, 100, true, "%d") != entry);

    xui::bind_info bind{'J', xui::bind_mode::toggle, true};
    config::serial::json_to_bind({{"k", 1000}, {"m", 99}}, bind);
    assert(bind.key == 0 && !bind.active);

    // Settings registered after initialize() are absent from the defaults
    // snapshot, but their assignments must not leak into the next profile.
    xui::setting late{false, {}, "late bind regression", "ragebot"};
    late.bind.excludes = &rage.enabled;
    const auto arm_late = [&] {
        late.bind.key = 'L';
        late.bind.mode = xui::bind_mode::hold_on;
        late.bind.active = true;
        xui::ctx().active_keybind = 123;
    };
    const auto cleared = [&] {
        assert(late.bind.key == 0 && !late.bind.active);
        assert(late.bind.excludes == &rage.enabled);
        assert(xui::ctx().active_keybind == 0);
        for (auto* setting : xui::binds::all())
            assert(!setting || setting->bind.key == 0);
        assert(xui::slider_binds::serialize().empty());
    };
    arm_late();
    assert(!config::from_json({{"version", config::k_version}, {"fields", nullptr}}));
    assert(late.bind.key == 'L'); // Reject invalid input before clearing assignments.
    assert(config::from_json({{"version", config::k_version}, {"fields", nlohmann::json::object()}}));
    cleared();
    assert(config::from_json(saved));
    assert(config::to_json() == saved);
    arm_late();
    assert(config::from_json_delta({{"v", config::k_version}, {"f", nlohmann::json::object()}}));
    cleared();
    assert(config::from_json_delta(delta));
    assert(config::to_json() == saved);
    arm_late();
    config::apply_blank_profile();
    cleared();
    xui::binds::unregister_setting(&late);
}
