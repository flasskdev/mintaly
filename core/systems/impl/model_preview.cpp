#include <pch/pch.hpp>
#include <core/systems/systems.hpp>
#include <core/features/misc/misc.hpp>
#include <utilities/addresses/addresses.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/logging/logging.hpp>
#include <external/nlohmann/json.hpp>
#include "native_preview_script.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>

namespace systems {
namespace {
using panel = features::misc::c_ui_panel;
using ui_engine = features::misc::c_ui_engine;
using panel_entry = features::misc::panel_data_t;

bool valid_panel(panel* value) {
    if (!value) return false;
    return memory::safe_read<std::uintptr_t>(reinterpret_cast<std::uintptr_t>(value)).value_or(0) != 0;
}

panel* find_root(ui_engine* engine) {
    // Re-resolve every update. HUDs can be replaced by team selection without
    // a level-change notification. Never run a script on a cached raw pointer.
    if (addresses::globals::hud) {
        const auto hud = memory::safe_read<std::uintptr_t>(addresses::globals::hud).value_or(0);
        if (hud) {
            auto* p = memory::safe_read<panel*>(hud + 0x8).value_or(nullptr);
            if (valid_panel(p)) return p;
        }
    }
    const auto base = reinterpret_cast<std::uintptr_t>(engine);
    const auto count = memory::safe_read<int>(base + offsetof(ui_engine, m_panel_count)).value_or(0);
    const auto entries = memory::safe_read<std::uintptr_t>(base + offsetof(ui_engine, m_panels_array)).value_or(0);
    if (!entries || count <= 0 || count > 16384) return nullptr;
    for (int i = 0; i < count; ++i) {
        const auto entry = memory::safe_read<panel_entry>(entries + i * sizeof(panel_entry));
        if (!entry || !entry->m_visible || !valid_panel(entry->m_panel)) continue;
        const auto p = reinterpret_cast<std::uintptr_t>(entry->m_panel);
        const auto name = memory::safe_read<std::uintptr_t>(p + offsetof(panel, m_panel_name)).value_or(0);
        if (name && memory::read_string(name, 64) == "CSGOMainMenu") return entry->m_panel;
    }
    return nullptr;
}

const char* kind_name(model_preview::kind value) {
    switch (value) {
        case model_preview::kind::knife: return "knife";
        case model_preview::kind::gloves: return "gloves";
        case model_preview::kind::agent: return "agent";
        case model_preview::kind::music: return "music";
        default: return "weapon";
    }
}

bool texture_name_matches(const std::string& name, const std::string& expected) {
    if (expected.empty()) return false;
    const auto at = name.find(expected);
    if (at == std::string::npos) return false;
    // Some builds prefix the resource path and append a numeric panel suffix.
    auto tail = name.substr(at + expected.size());
    if (tail.empty()) return true;
    return tail.size() > 1 && tail.front() == '_' &&
        std::all_of(tail.begin() + 1, tail.end(), [](char c) { return c >= '0' && c <= '9'; });
}
}

bool model_preview::request::same_asset(const request& o) const {
    return type == o.type && def_index == o.def_index && paint_kit == o.paint_kit &&
        music_kit == o.music_kit && team == o.team && model_path == o.model_path &&
        width == o.width && height == o.height;
}

bool model_preview::initialize() {
    std::lock_guard lock(mutex_);
    initialized_ = true;
    session_ = std::to_string(GetTickCount64());
    status_ = "Waiting for native preview hooks...";
    return true; // Missing optional hooks must not abort plugin initialization.
}

void model_preview::set_capture_available(bool available) {
    std::lock_guard lock(mutex_);
    capture_available_ = available;
    if (!available) {
        texture_.Reset();
        status_ = "Native 3D unavailable: preview hook not resolved.";
    }
}

void model_preview::submit(request value) {
    if (!std::isfinite(value.yaw)) value.yaw = 0;
    if (!std::isfinite(value.pitch)) value.pitch = 0;
    value.yaw = std::remainder(value.yaw, 360.0f);
    value.pitch = std::clamp(value.pitch, -85.0f, 85.0f);
    value.width = std::clamp(value.width, 64, 1024);
    value.height = std::clamp(value.height, 64, 1024);
    const bool invalid = value.def_index < 0 || value.def_index > 65535 ||
        value.paint_kit < 0 || value.music_kit < 0 || value.model_path.size() > 1024 ||
        (value.type != kind::agent && value.type != kind::music && value.def_index == 0);
    std::lock_guard lock(mutex_);
    if (!initialized_) return;
    if (invalid) {
        visible_ = false;
        wanted_.reset();
        texture_.Reset();
        status_ = "Invalid native preview parameters.";
        return;
    }
    const auto now = clock::now();
    if (!visible_ || !wanted_ || !wanted_->same_asset(value)) {
        ++generation_;
        expected_name_ = "mintaly_preview_" + session_ + "_" + std::to_string(generation_);
        issued_generation_ = 0;
        changed_at_ = now;
        texture_.Reset();
        if (capture_available_) status_ = "Loading original CS2 model...";
    }
    wanted_ = std::move(value);
    visible_ = true;
    last_submit_ = now;
}

void model_preview::hide() {
    std::lock_guard lock(mutex_);
    visible_ = false;
    texture_.Reset();
}

void model_preview::update() {
    const auto now = clock::now();
    if (now - last_script_ < std::chrono::milliseconds(33)) return;
    last_script_ = now;

    request value;
    std::uint64_t generation;
    std::string name;
    bool visible;
    {
        std::lock_guard lock(mutex_);
        if (!initialized_) return;
        visible = visible_ && capture_available_ && wanted_.has_value() &&
            now - last_submit_ < std::chrono::milliseconds(300);
        if (!visible) {
            visible_ = false;
            texture_.Reset();
        }
        if (visible && now - changed_at_ < std::chrono::milliseconds(80)) return;
        if (wanted_) value = *wanted_;
        generation = generation_;
        name = expected_name_;
    }
    if (!visible && !root_panel_) return;
    if (!addresses::globals::panorama) return;
    auto* panorama = reinterpret_cast<features::misc::c_panorama_ui_engine*>(addresses::globals::panorama);
    auto* engine = panorama->get_ui_engine();
    if (!engine) return;
    auto* root = find_root(engine);
    if (!root) {
        root_panel_ = nullptr;
        std::lock_guard lock(mutex_);
        texture_.Reset();
        issued_generation_ = 0;
        if (visible) status_ = "Native 3D waiting for HUD / main menu.";
        return;
    }
    const bool root_changed = root != root_panel_;
    if (root_changed) {
        std::lock_guard lock(mutex_);
        if (generation != generation_) return;
        texture_.Reset();
        ++generation_;
        generation = generation_;
        name = expected_name_ = "mintaly_preview_" + session_ + "_" + std::to_string(generation_);
        issued_generation_ = 0;
    }
    root_panel_ = root;
    try {
        // Reinstall idempotently once a second, to recover a reloaded JS context.
        if (root_changed || now - last_bootstrap_ > std::chrono::seconds(1)) {
            engine->run_script(root, native_preview_detail::bootstrap);
            last_bootstrap_ = now;
        }
        nlohmann::json args = {{"visible", visible}};
        if (visible) {
            args.update({{"texture", name}, {"kind", kind_name(value.type)},
                {"def", value.def_index}, {"paint", value.paint_kit}, {"music", value.music_kit},
                {"model", value.model_path}, {"width", value.width}, {"height", value.height},
                {"yaw", value.yaw}, {"pitch", value.pitch}});
        }
        const std::string script =
            "(function(){var s=$.GetContextPanel().Data().mintalyNativePreview;"
            "if(s)s.submit(" + args.dump() + ");})();";
        {
            std::lock_guard lock(mutex_);
            if (generation != generation_) return; // Superseded while finding root.
            issued_generation_ = visible ? generation : 0;
        }
        engine->run_script(root, script.c_str());
        if (!visible) root_panel_ = nullptr;
        std::lock_guard lock(mutex_);
        if (visible && !texture_ && now - changed_at_ > std::chrono::seconds(3))
            status_ = "No native frame. Check Panorama console / hook compatibility.";
    } catch (const std::exception& e) {
        std::lock_guard lock(mutex_);
        issued_generation_ = 0;
        texture_.Reset();
        status_ = "Unable to configure native preview.";
        logging::console::print("[model_preview] {}", e.what());
    }
}

void model_preview::capture_resource(std::uintptr_t handle, char alternate_view,
                                    ID3D11ShaderResourceView* srv) {
    if (!handle || !srv || alternate_view != 0) return;
    {
        std::lock_guard lock(mutex_);
        if (!initialized_ || !capture_available_ || !visible_ || issued_generation_ != generation_) return;
    }
    // This layout belongs to the SRV resolver, not a guessed scene-layer offset.
    const auto base = memory::safe_read<std::uintptr_t>(handle).value_or(0);
    if (!base) return;
    const auto string_ptr = memory::safe_read<std::uintptr_t>(base + sizeof(std::uintptr_t)).value_or(0);
    if (!string_ptr) return;
    const auto text_ptr = memory::safe_read<std::uintptr_t>(string_ptr).value_or(0);
    if (!text_ptr) return;
    const auto name = memory::read_string(text_ptr, 256);
    std::lock_guard lock(mutex_);
    if (!initialized_ || !visible_ || issued_generation_ != generation_ ||
        !texture_name_matches(name, expected_name_)) return;
    // Retain a COM reference. The render thread takes a second lease that stays
    // alive until its deferred xdraw image command has been submitted.
    texture_ = srv;
    last_capture_ = clock::now();
    status_ = "Original CS2 model / catalogue finish";
}

model_preview::texture_ref model_preview::acquire(ID3D11Device* device) {
    std::lock_guard lock(mutex_);
    if (!device || !visible_ || !texture_ || issued_generation_ != generation_) return {};
    if (clock::now() - last_capture_ > std::chrono::seconds(2)) {
        texture_.Reset();
        status_ = "Native preview paused. Waiting for a fresh frame...";
        return {};
    }
    Microsoft::WRL::ComPtr<ID3D11Device> owner;
    texture_->GetDevice(owner.GetAddressOf());
    if (owner.Get() != device) return {};
    return texture_;
}

std::string model_preview::status() const {
    std::lock_guard lock(mutex_);
    return status_;
}

void model_preview::reset() {
    std::lock_guard lock(mutex_);
    visible_ = false;
    wanted_.reset();
    texture_.Reset();
    ++generation_;
    issued_generation_ = 0;
    expected_name_.clear();
}

void model_preview::shutdown() {
    std::lock_guard lock(mutex_);
    initialized_ = visible_ = capture_available_ = false;
    texture_.Reset();
    wanted_.reset();
    // No Panorama calls on a loader / unload thread. The JS lease expires.
}

std::uintptr_t model_preview::resource_view_address() {
    // Optional, version-dependent integration. Never fall back to capturing a
    // random scene texture. An unresolved signature leaves the labelled 2D view.
    return memory::resolve_pattern(
        "rendersystemdx11.dll:48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 48 89 4C 24 ? "
        "55 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 33 DB 4D 0F BE F8 "
        "89 5D ? 45 0F B6 E1 48 8B 02 4C 8B F2 4C 8B 2D ? ? ? ? 48 85 C0 "
        "0F 84 ? ? ? ? 8B 40 ? 85 C0 0F 8E ? ? ? ? 48 8B 02 48 8B 30 48 85 F6 "
        "0F 84 ? ? ? ? 41 80 FF");
}
}
