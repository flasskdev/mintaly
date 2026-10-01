#include <pch/pch.hpp>
#include <core/systems/native_preview.hpp>
#include <core/systems/impl/native_preview_script.hpp>
#include <core/features/features.hpp>
#include <utilities/addresses/addresses.hpp>
#include <utilities/cstypes.hpp>
#include <utilities/cosmetic_model.hpp>
#include <utilities/diag.hpp>
#include <utilities/memory/memory.hpp>

namespace systems {

namespace {
// Private capture-generation state lives beside the singleton implementation
// so adding arbitration data does not change model_preview's public ABI/layout.
struct preview_candidate_filter {
    std::array<std::uintptr_t, 64> preexisting_entities{};
    std::size_t preexisting_count{};
    std::chrono::steady_clock::time_point warmup_until{};
};
preview_candidate_filter g_preview_candidate_filter{};
}

bool model_preview::request::same_asset(const request& o) const {
    return type == o.type && def_index == o.def_index && paint_kit == o.paint_kit &&
        music_kit == o.music_kit && team == o.team && wear == o.wear && seed == o.seed &&
        stattrak == o.stattrak && stattrak_count == o.stattrak_count && name_tag == o.name_tag &&
        model_path == o.model_path &&
        width == o.width && height == o.height && screen_width == o.screen_width &&
        screen_height == o.screen_height && background_rgb == o.background_rgb &&
        x == o.x && y == o.y &&
        yaw == o.yaw && pitch == o.pitch && visible == o.visible;
}

bool model_preview::initialize() {
    std::lock_guard lock(mutex_);
    initialized_ = true;
    capture_available_ = false;
    bootstrap_sent_ = false;
    connected_ = false;
    pose_capture_requested_.store(false, std::memory_order_release);
    agent_pose_ = {};
    pose_entity_ = 0;
    g_preview_candidate_filter = {};
    pose_basis_x_ = 1.0f;
    pose_basis_y_ = 0.0f;
    pose_basis_valid_ = false;
    wanted_.reset();
    last_sent_.reset();
    last_script_ = {};
    status_ = "Native CS2 agent preview is ready.";
    diag::writef(diag::level::info, "[model-preview] initialized build=skin-preview-20260930-r5");
    return true;
}

void model_preview::set_capture_available(bool available) {
    std::lock_guard lock(mutex_);
    const bool changed = capture_available_ != available;
    capture_available_ = available;
    if (changed)
        diag::writef(diag::level::info, "[model-preview] frame-stage hook available=%d", static_cast<int>(available));
    if (!available) {
        bootstrap_sent_ = false;
        connected_ = false;
        agent_pose_ = {};
        pose_entity_ = 0;
        g_preview_candidate_filter = {};
        pose_basis_x_ = 1.0f;
        pose_basis_y_ = 0.0f;
        pose_basis_valid_ = false;
        status_ = "Native preview waits for the game frame hook.";
    }
}

void model_preview::submit(request value) {
    if (!std::isfinite(value.x) || !std::isfinite(value.y) ||
        !std::isfinite(value.yaw) || !std::isfinite(value.pitch) || !std::isfinite(value.wear))
        return;
    value.width = std::clamp(value.width, 64, 1600);
    value.height = std::clamp(value.height, 64, 1600);
    value.screen_width = std::clamp(value.screen_width, 64, 8192);
    value.screen_height = std::clamp(value.screen_height, 64, 8192);
    value.x = std::round(value.x);
    value.y = std::round(value.y);
    value.yaw = std::remainder(value.yaw, 360.0f);
    value.pitch = std::clamp(value.pitch, -85.0f, 85.0f);
    value.wear = std::clamp(value.wear, 0.0f, 1.0f);
    value.seed = std::clamp(value.seed, 0, 1000);
    value.stattrak_count = std::max(0, value.stattrak_count);
    value.visible = true;
    std::lock_guard lock(mutex_);
    if (!initialized_) return;
    if (wanted_ && wanted_->same_asset(value)) return;
    const bool was_visible = wanted_ && wanted_->visible;
    const bool preview_asset_changed = !wanted_ || wanted_->type != value.type ||
        wanted_->def_index != value.def_index || wanted_->team != value.team ||
        wanted_->model_path != value.model_path;
    const bool new_preview_instance = value.visible && !was_visible;
    const bool log_request = !wanted_ || !wanted_->visible ||
        wanted_->type != value.type || wanted_->def_index != value.def_index ||
        wanted_->team != value.team || wanted_->model_path != value.model_path;
    wanted_ = std::move(value);
    pose_capture_requested_.store(
        wanted_->visible && wanted_->type == kind::agent, std::memory_order_release);
    if (preview_asset_changed || new_preview_instance) {
        connected_ = false;
        agent_pose_ = {};
        pose_entity_ = 0;
        g_preview_candidate_filter.preexisting_entities.fill(0);
        g_preview_candidate_filter.preexisting_count = 0;
        // Let the existing menu scene render once before Panorama creates the
        // isolated panel. Its matching lobby actor must not win first capture.
        g_preview_candidate_filter.warmup_until = clock::now() + std::chrono::milliseconds(400);
        pose_basis_x_ = 1.0f;
        pose_basis_y_ = 0.0f;
        pose_basis_valid_ = false;
    }
    if (log_request)
        diag::writef(diag::level::info,
            "[model-preview] request submitted visible=%d team=%d def=%d size=%dx%d screen=%dx%d pos=%.1f,%.1f model=%s hook=%d",
            static_cast<int>(wanted_->visible), wanted_->team, wanted_->def_index,
            wanted_->width, wanted_->height, wanted_->screen_width, wanted_->screen_height,
            wanted_->x, wanted_->y, wanted_->model_path.c_str(), static_cast<int>(capture_available_));
}

void model_preview::hide() {
    std::lock_guard lock(mutex_);
    if (!wanted_ || !wanted_->visible) return;
    wanted_->visible = false;
    connected_ = false;
    status_ = "CS2 item preview closed.";
    pose_capture_requested_.store(false, std::memory_order_release);
    agent_pose_ = {};
    pose_entity_ = 0;
    g_preview_candidate_filter.preexisting_entities.fill(0);
    g_preview_candidate_filter.preexisting_count = 0;
    pose_basis_valid_ = false;
}

bool model_preview::item_preview_active() const {
    std::lock_guard lock(mutex_);
    return wanted_ && wanted_->visible;
}

bool model_preview::wants_agent_pose() const noexcept {
    return pose_capture_requested_.load(std::memory_order_acquire);
}

void model_preview::capture_agent_pose(std::uintptr_t entity) {
    if (!entity || !wants_agent_pose())
        return;

    const auto report_pose = [entity](const char* state, std::size_t points,
        float width_ratio, float height_ratio) {
        static std::atomic<std::int64_t> next_log_ms{};
        const auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            clock::now().time_since_epoch()).count();
        auto expected = next_log_ms.load(std::memory_order_relaxed);
        if (now_ms >= expected && next_log_ms.compare_exchange_strong(
            expected, now_ms + 2500, std::memory_order_relaxed)) {
            diag::writef(diag::level::info,
                "[preview-esp] live-pose=%s entity=0x%llx points=%llu size=%.2fx%.2f",
                state, static_cast<unsigned long long>(entity),
                static_cast<unsigned long long>(points), width_ratio, height_ratio);
        }
    };

    request value{};
    float basis_x{1.0f}, basis_y{};
    bool has_basis{};
    {
        std::lock_guard lock(mutex_);
        if (!wanted_ || !wanted_->visible || wanted_->type != kind::agent)
            return;
        if (pose_entity_ && pose_entity_ != entity)
            return;
        value = *wanted_;
        basis_x = pose_basis_x_;
        basis_y = pose_basis_y_;
        has_basis = pose_basis_valid_;
    }

    // PreviewPlayer is also used by the main-menu vanity scene. Only accept
    // the actor whose actual model matches the panel's selected agent.
    const auto scene_node_offset = SCHEMA("C_BaseEntity", "m_pGameSceneNode"_hash);
    const auto model_state_offset = SCHEMA("CSkeletonInstance", "m_modelState"_hash);
    const auto model_name_offset = SCHEMA("CModelState", "m_ModelName"_hash);
    const auto scene_node = scene_node_offset
        ? memory::safe_read<std::uintptr_t>(entity + scene_node_offset).value_or(0) : 0;
    const auto model_state = scene_node && model_state_offset ? scene_node + model_state_offset : 0;
    const auto model_name = model_state && model_name_offset
        ? memory::safe_read<std::uintptr_t>(model_state + model_name_offset).value_or(0) : 0;
    const auto actual_model = model_name ? memory::read_string(model_name, 512) : std::string{};
    if (actual_model.empty() || value.model_path.empty() ||
        !cosmetic_model::matches(actual_model, value.model_path)) {
        report_pose("model-mismatch", 0, 0.0f, 0.0f);
        return;
    }

    // MapPlayerPreviewPanel assigns generated entity names such as
    // "dynamic_player0" and ignores its playername attribute. The main-menu
    // vanity actor keeps a different name (vanity_character); use the generated
    // prefix to accept the panel actor without accidentally capturing lobby pose.
    const auto identity = memory::safe_read<std::uintptr_t>(entity + 0x10).value_or(0);
    const auto identity_name_offset = SCHEMA("CEntityIdentity", "m_name"_hash);
    const auto identity_name_ptr = identity
        ? memory::safe_read<std::uintptr_t>(identity + identity_name_offset).value_or(0) : 0;
    const auto actual_name = identity_name_ptr
        ? memory::read_string(identity_name_ptr, 128) : std::string{};
    if (!actual_name.starts_with(native_preview_detail::agent_entity_name_prefix)) {
        static std::atomic<std::int64_t> next_identity_log_ms{};
        const auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            clock::now().time_since_epoch()).count();
        auto expected_log_ms = next_identity_log_ms.load(std::memory_order_relaxed);
        if (now_ms >= expected_log_ms && next_identity_log_ms.compare_exchange_strong(
            expected_log_ms, now_ms + 2500, std::memory_order_relaxed)) {
            diag::writef(diag::level::info,
                "[preview-esp] candidate rejected entity=0x%llx name='%s' expected-prefix='%s' model='%s'",
                static_cast<unsigned long long>(entity), actual_name.c_str(),
                native_preview_detail::agent_entity_name_prefix, actual_model.c_str());
        }
        report_pose("identity-mismatch", 0, 0.0f, 0.0f);
        return;
    }

    const auto team_offset = SCHEMA("C_BaseEntity", "m_iTeamNum"_hash);
    if (team_offset) {
        const auto actual_team = memory::safe_read<std::uint8_t>(entity + team_offset).value_or(0);
        if ((actual_team == 2 || actual_team == 3) && actual_team != value.team) {
            report_pose("team-mismatch", 0, 0.0f, 0.0f);
            return;
        }
    }

    // The menu's vanity actor can share model, team and generated name with
    // the separately rendered MapPlayerPreviewPanel actor. Snapshot matching
    // actors while the Panorama request is pending, then only allow a new
    // entity created by the preview panel to become the pose source.
    bool is_preexisting{};
    bool is_waiting_for_panel{};
    {
        std::lock_guard lock(mutex_);
        if (!wanted_ || !wanted_->visible || wanted_->type != kind::agent ||
            wanted_->team != value.team || wanted_->def_index != value.def_index ||
            wanted_->model_path != value.model_path)
            return;
        if (pose_entity_ && pose_entity_ != entity)
            return;

        is_waiting_for_panel = !connected_;
        for (std::size_t i = 0; i < g_preview_candidate_filter.preexisting_count; ++i) {
            if (g_preview_candidate_filter.preexisting_entities[i] == entity) {
                is_preexisting = true;
                break;
            }
        }
        if (is_waiting_for_panel && !is_preexisting &&
            g_preview_candidate_filter.preexisting_count < g_preview_candidate_filter.preexisting_entities.size()) {
            g_preview_candidate_filter.preexisting_entities[g_preview_candidate_filter.preexisting_count++] = entity;
            is_preexisting = true;
        }
    }
    if (is_waiting_for_panel || is_preexisting) {
        report_pose(is_waiting_for_panel ? "pre-panel-candidate" : "excluded-lobby-candidate",
            0, 0.0f, 0.0f);
        return;
    }

    const auto skeleton = g_bones.get_skeleton(entity);
    constexpr auto pelvis_id = cstypes::bone_ids::pelvis;
    constexpr auto left_shoulder_id = cstypes::bone_ids::left_shoulder;
    constexpr auto right_shoulder_id = cstypes::bone_ids::right_shoulder;
    const auto finite_point = [](const math::vector3& p) {
        return std::isfinite(p.x) && std::isfinite(p.y) && std::isfinite(p.z);
    };
    const auto nonzero_point = [](const math::vector3& p) {
        return std::fabs(p.x) + std::fabs(p.y) + std::fabs(p.z) > 0.001f;
    };
    const auto distance_sq = [](const math::vector3& a, const math::vector3& b) {
        const auto dx = a.x - b.x;
        const auto dy = a.y - b.y;
        const auto dz = a.z - b.z;
        return dx * dx + dy * dy + dz * dz;
    };

    // Preview bones are in the isolated MapPlayerPreviewPanel scene. The
    // gameplay view matrix projects them to a near-zero rectangle, so build a
    // stable panel projection from the live animated bone positions instead.
    std::array<bool, 27> candidates{};
    std::size_t candidate_count{};
    const auto pelvis = skeleton[pelvis_id].position;
    for (std::size_t i = 0; i < skeleton.size(); ++i) {
        const auto& p = skeleton[i].position;
        if (!finite_point(p) || (!nonzero_point(p) && i != pelvis_id))
            continue;
        candidates[i] = true;
        ++candidate_count;
    }

    if (candidate_count < 10) {
        report_pose("bones-unavailable", candidate_count, 0.0f, 0.0f);
        return;
    }

    math::vector3 origin{};
    std::size_t pelvis_near_count{};
    for (std::size_t i = 0; i < skeleton.size(); ++i) {
        if (candidates[i] && distance_sq(skeleton[i].position, pelvis) < 180.0f * 180.0f)
            ++pelvis_near_count;
    }
    if (pelvis_near_count >= 10) {
        origin = pelvis;
    } else {
        for (std::size_t i = 0; i < skeleton.size(); ++i) {
            if (!candidates[i]) continue;
            origin.x += skeleton[i].position.x;
            origin.y += skeleton[i].position.y;
            origin.z += skeleton[i].position.z;
        }
        const auto inv_count = 1.0f / static_cast<float>(candidate_count);
        origin.x *= inv_count;
        origin.y *= inv_count;
        origin.z *= inv_count;
    }

    std::array<bool, 27> valid_bones{};
    std::size_t valid_count{};
    float min_z = (std::numeric_limits<float>::max)();
    float max_z = (std::numeric_limits<float>::lowest)();
    for (std::size_t i = 0; i < skeleton.size(); ++i) {
        if (!candidates[i] || distance_sq(skeleton[i].position, origin) >= 180.0f * 180.0f)
            continue;
        valid_bones[i] = true;
        min_z = std::min(min_z, skeleton[i].position.z);
        max_z = std::max(max_z, skeleton[i].position.z);
        ++valid_count;
    }

    const auto world_height = max_z - min_z;
    if (valid_count < 10 || !std::isfinite(world_height) || world_height < 24.0f || world_height > 180.0f) {
        report_pose("bone-range-rejected", valid_count, 0.0f, 0.0f);
        return;
    }

    constexpr float k_deg2rad = 3.14159265358979323846f / 180.0f;
    const float yaw_rad = value.yaw * k_deg2rad;
    basis_x = -std::sin(yaw_rad);
    basis_y = std::cos(yaw_rad);

    float min_horizontal = (std::numeric_limits<float>::max)();
    float max_horizontal = (std::numeric_limits<float>::lowest)();
    for (std::size_t i = 0; i < skeleton.size(); ++i) {
        if (!valid_bones[i]) continue;
        const auto& p = skeleton[i].position;
        const auto horizontal = (p.x - origin.x) * basis_x + (p.y - origin.y) * basis_y;
        min_horizontal = std::min(min_horizontal, horizontal);
        max_horizontal = std::max(max_horizontal, horizontal);
    }
    const auto world_width = max_horizontal - min_horizontal;
    if (!std::isfinite(world_width) || world_width < 0.5f) {
        report_pose("bone-width-rejected", valid_count, 0.0f, 0.0f);
        return;
    }

    agent_pose_snapshot snapshot{};
    float min_x = value.x + static_cast<float>(value.width);
    float min_y = value.y + static_cast<float>(value.height);
    float max_x = value.x;
    float max_y = value.y;
    // In warehouse_vanity with cam_char_inspect_wide, the model spans ~67.5% of panel
    // height from foot bone to head bone (eyes), with feet placed at 94.5% of panel height.
    const auto scale_y = std::min(
        static_cast<float>(value.height) * 0.675f / world_height,
        static_cast<float>(value.width) * 0.85f / world_width);
    // Square pixel aspect ratio ensures limbs and shoulders maintain true anatomical 1:1 scale:
    const auto scale_x = scale_y;
    const auto horizontal_center = (min_horizontal + max_horizontal) * 0.5f;
    const auto panel_center_x = value.x + static_cast<float>(value.width) * 0.50f;
    const auto panel_bottom = value.y + static_cast<float>(value.height) * 0.945f;

    for (std::size_t i = 0; i < skeleton.size(); ++i) {
        if (!valid_bones[i])
            continue;

        const auto& p = skeleton[i].position;
        const auto horizontal = (p.x - origin.x) * basis_x + (p.y - origin.y) * basis_y;
        auto& point = snapshot.bones[i];
        point.x = panel_center_x + (horizontal - horizontal_center) * scale_x;
        point.y = panel_bottom - (p.z - min_z) * scale_y;
        point.valid = true;
        min_x = std::min(min_x, point.x);
        min_y = std::min(min_y, point.y);
        max_x = std::max(max_x, point.x);
        max_y = std::max(max_y, point.y);
    }

    const auto pose_width = max_x - min_x;
    const auto pose_height = max_y - min_y;
    const auto width_ratio = value.width > 0 ? pose_width / value.width : 0.0f;
    const auto height_ratio = value.height > 0 ? pose_height / value.height : 0.0f;
    const bool plausible = pose_width >= value.width * 0.015f && pose_width <= value.width * 0.90f &&
        pose_height >= value.height * 0.20f && pose_height <= value.height * 0.90f &&
        min_x >= value.x && max_x <= value.x + value.width &&
        min_y >= value.y && max_y <= value.y + value.height;
    if (!plausible) {
        report_pose("local-pose-rejected", valid_count, width_ratio, height_ratio);
        return;
    }

    const float pad_x = std::max(6.0f, pose_width * 0.06f);
    // Head bone is at eye level; pad above head bone by 8% of pose height to encompass headwear:
    const float pad_top = std::max(12.0f, pose_height * 0.08f);
    const float pad_bottom = std::max(4.0f, pose_height * 0.02f);
    snapshot.left = min_x - pad_x;
    snapshot.top = min_y - pad_top;
    snapshot.right = max_x + pad_x;
    snapshot.bottom = max_y + pad_bottom;
    snapshot.captured_at = clock::now();
    snapshot.valid = true;

    {
        std::lock_guard lock(mutex_);
        if (!wanted_ || !wanted_->visible || wanted_->type != kind::agent ||
            wanted_->team != value.team || wanted_->def_index != value.def_index ||
            wanted_->model_path != value.model_path)
            return;
        if (pose_entity_ && pose_entity_ != entity)
            return;
        if (!pose_entity_) {
            pose_entity_ = entity;
            if (!pose_basis_valid_) {
                pose_basis_x_ = basis_x;
                pose_basis_y_ = basis_y;
                pose_basis_valid_ = true;
            }
        }
        agent_pose_ = snapshot;
    }

    report_pose("local-pose-live", valid_count, width_ratio, height_ratio);
}

model_preview::agent_pose_snapshot model_preview::get_agent_pose() const {
    std::lock_guard lock(mutex_);
    auto pose = agent_pose_;
    if (!pose.valid || pose.captured_at == clock::time_point{} ||
        clock::now() - pose.captured_at > std::chrono::milliseconds(100))
        return {};
    return pose;
}

bool model_preview::is_agent_preview_entity(std::uintptr_t entity) const {
    if (!entity || !pose_capture_requested_.load(std::memory_order_acquire))
        return false;
    std::lock_guard lock(mutex_);
    return pose_entity_ == entity;
}

void model_preview::update() {
    request value{};
    bool needs_bootstrap{};
    {
        std::lock_guard lock(mutex_);
        if (!initialized_ || !wanted_) return;
        if (!capture_available_ && wanted_->visible) {
            if (wanted_->visible) {
                static auto next_gate_log = clock::time_point{};
                const auto now = clock::now();
                if (now >= next_gate_log) {
                    next_gate_log = now + std::chrono::seconds(2);
                    diag::writef(diag::level::warning,
                        "[model-preview] update gated initialized=%d frame-stage-hook=%d request=%d",
                        static_cast<int>(initialized_), static_cast<int>(capture_available_),
                        static_cast<int>(wanted_.has_value()));
                }
            }
            return;
        }
        value = *wanted_;
        if (!value.visible && (!last_sent_ || !last_sent_->visible)) return;
        const auto now = clock::now();
        if (value.visible && value.type == kind::agent && !connected_ &&
            g_preview_candidate_filter.warmup_until != clock::time_point{} &&
            now < g_preview_candidate_filter.warmup_until)
            return;
        const bool unchanged = last_sent_ && last_sent_->same_asset(value);
        const bool selection_changed = !last_sent_ || last_sent_->type != value.type ||
            last_sent_->def_index != value.def_index || last_sent_->team != value.team ||
            last_sent_->paint_kit != value.paint_kit || last_sent_->music_kit != value.music_kit ||
            last_sent_->wear != value.wear || last_sent_->seed != value.seed ||
            last_sent_->stattrak != value.stattrak || last_sent_->stattrak_count != value.stattrak_count ||
            last_sent_->name_tag != value.name_tag || last_sent_->model_path != value.model_path;
        const bool visibility_changed = !last_sent_ || last_sent_->visible != value.visible;
        const bool layout_changed = !last_sent_ || last_sent_->width != value.width ||
            last_sent_->height != value.height || last_sent_->screen_width != value.screen_width ||
            last_sent_->screen_height != value.screen_height || last_sent_->background_rgb != value.background_rgb ||
            last_sent_->x != value.x || last_sent_->y != value.y;
        const bool rotation_changed = !last_sent_ || last_sent_->yaw != value.yaw || last_sent_->pitch != value.pitch;
        const auto min_interval = !value.visible ? std::chrono::milliseconds(0) :
            !bootstrap_sent_ ? std::chrono::milliseconds(250) :
            unchanged ? std::chrono::milliseconds(500) :
            (selection_changed || visibility_changed || rotation_changed) ? std::chrono::milliseconds(16) :
            layout_changed ? std::chrono::milliseconds(0) : std::chrono::milliseconds(500);
        if (unchanged && !value.visible) return;
        if (last_script_ != clock::time_point{} && now - last_script_ < min_interval) return;
        // Panorama script execution is synchronous on the game UI thread. Keep
        // layout updates instant (0ms) so the preview tracks the menu window 1:1 without dragging lag.
        needs_bootstrap = value.visible && !bootstrap_sent_;
        last_script_ = now;
    }

    auto& bridge = features::misc::g_scoreboard_weapons;
    if (needs_bootstrap) {
        if (!bridge.run_preview_script(native_preview_detail::bootstrap)) {
            std::lock_guard lock(mutex_);
            connected_ = false;
            status_ = "Waiting for a live CS2 Panorama panel.";
            return;
        }
        std::lock_guard lock(mutex_);
        bootstrap_sent_ = true;
        connected_ = false;
        diag::writef(diag::level::info, "[model-preview] Panorama bootstrap submitted");
    }

    const char* kind_name = "weapon";
    switch (value.type) {
    case model_preview::kind::knife: kind_name = "knife"; break;
    case model_preview::kind::gloves: kind_name = "gloves"; break;
    case model_preview::kind::agent: kind_name = "agent"; break;
    case model_preview::kind::music: kind_name = "music"; break;
    case model_preview::kind::weapon: default: break;
    }

    const nlohmann::json payload{
        {"visible", value.visible}, {"kind", kind_name},
        {"def", value.def_index}, {"paint", value.paint_kit},
        {"music", value.music_kit}, {"team", value.team},
        {"wear", value.wear}, {"seed", value.seed},
        {"stattrak", value.stattrak}, {"stattrak_count", value.stattrak_count},
        {"name_tag", value.name_tag},
        {"model", value.model_path}, {"width", value.width}, {"height", value.height},
        {"screen_width", value.screen_width}, {"screen_height", value.screen_height},
        {"background_rgb", value.background_rgb},
        {"x", value.x}, {"y", value.y}, {"yaw", value.yaw}, {"pitch", value.pitch},
        {"animation", "inventory-inspect"}
    };
    const auto script = std::string{"(function(){var q=$.GetContextPanel(),origin=q,r=null;"} +
        "for(var i=0;i<32&&q&&q.IsValid();i++){try{var d=q.Data(),s=d&&d.mintalyNativePreview;" +
        "if(s&&s.submit){r=s;break;}q=q.GetParent();}catch(e){break;}}" +
        "if(r&&r.submit){try{if(origin&&origin.IsValid())origin.Data().mintalyPreviewBridgeMissing=false;}catch(e){}r.submit(" +
        payload.dump() + ");}else{try{var d=origin&&origin.IsValid()?origin.Data():null;" +
        "if(d&&!d.mintalyPreviewBridgeMissing){d.mintalyPreviewBridgeMissing=true;" +
        "$.Msg('[mintaly preview] request ignored: bridge state missing');" +
        "try{if(typeof GameInterfaceAPI!=='undefined'&&typeof GameInterfaceAPI.ConsoleCommand==='function')" +
        "GameInterfaceAPI.ConsoleCommand('echoln \"[mintaly-js] request ignored: bridge state missing\"');}catch(x){}}}catch(e){}}})()";
    if (!bridge.run_preview_script(script)) {
        std::lock_guard lock(mutex_);
        connected_ = false;
        status_ = "CS2 Panorama panel changed; waiting to reconnect.";
        bootstrap_sent_ = false;
        return;
    }

    std::lock_guard lock(mutex_);
    const bool asset_changed = !last_sent_ || last_sent_->type != value.type ||
        last_sent_->def_index != value.def_index || last_sent_->team != value.team ||
        last_sent_->paint_kit != value.paint_kit || last_sent_->music_kit != value.music_kit ||
        last_sent_->wear != value.wear || last_sent_->seed != value.seed ||
        last_sent_->stattrak != value.stattrak || last_sent_->stattrak_count != value.stattrak_count ||
        last_sent_->name_tag != value.name_tag ||
        last_sent_->model_path != value.model_path;
    connected_ = value.visible;
    last_sent_ = value;
    if (value.type == kind::agent && asset_changed) {
        diag::writef(diag::level::info,
            "[model-preview] isolated-panel capture armed after excluding %llu pre-panel actor(s)",
            static_cast<unsigned long long>(g_preview_candidate_filter.preexisting_count));
    }
    status_ = value.visible ? "CS2 native item preview is active." : "CS2 item preview closed.";
    if (asset_changed)
        diag::writef(diag::level::info,
            "[model-preview] request script submitted visible=%d team=%d def=%d paint=%d wear=%.4f seed=%d stattrak=%d:%d yaw=%.1f pitch=%.1f",
            static_cast<int>(value.visible), value.team, value.def_index, value.paint_kit,
            value.wear, value.seed, static_cast<int>(value.stattrak), value.stattrak_count,
            value.yaw, value.pitch);
}

void model_preview::reset() {
    std::lock_guard lock(mutex_);
    pose_capture_requested_.store(false, std::memory_order_release);
    agent_pose_ = {};
    pose_entity_ = 0;
    g_preview_candidate_filter = {};
    pose_basis_x_ = 1.0f;
    pose_basis_y_ = 0.0f;
    pose_basis_valid_ = false;
    wanted_.reset();
    last_sent_.reset();
    bootstrap_sent_ = false;
    connected_ = false;
    last_script_ = {};
}

void model_preview::shutdown() {
    std::lock_guard lock(mutex_);
    pose_capture_requested_.store(false, std::memory_order_release);
    agent_pose_ = {};
    pose_entity_ = 0;
    g_preview_candidate_filter = {};
    pose_basis_x_ = 1.0f;
    pose_basis_y_ = 0.0f;
    pose_basis_valid_ = false;
    initialized_ = false;
    capture_available_ = false;
    wanted_.reset();
    last_sent_.reset();
    bootstrap_sent_ = false;
    connected_ = false;
}

bool model_preview::is_enemy_preview(int local_team) const {
    std::lock_guard lock(mutex_);
    if (local_team != 2 && local_team != 3)
        return true;
    if (!wanted_ || !wanted_->visible || wanted_->type != kind::agent)
        return true;
    return wanted_->team != local_team;
}

bool model_preview::connected() const {
    std::lock_guard lock(mutex_);
    return connected_;
}

model_preview::texture_ref model_preview::acquire(ID3D11Device*) {
    return {};
}

std::string model_preview::status() const {
    std::lock_guard lock(mutex_);
    return status_;
}

void model_preview::capture_resource(std::uintptr_t, char, ID3D11ShaderResourceView*, const char*) {
}

std::uintptr_t model_preview::resource_view_address() {
    return 0;
}

}
