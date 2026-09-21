#include <pch/pch.hpp>
#include "sync_http.hpp"
#include "skin_sync.hpp"
#include "changer.hpp"
#include "preview_scene.hpp"
#include <utilities/steam/steam.hpp>
#include <utilities/lifecycle.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <core/systems/systems.hpp>
#include <external/nlohmann/json.hpp>

namespace features::changer {
namespace {
    constexpr std::uint64_t steam_base = 76561197960265728ull;
    constexpr auto profile_ttl = std::chrono::seconds(30);
    bool fresh(const remote_player_skin& skin) {
        return std::chrono::steady_clock::now() - skin.last_updated < profile_ttl;
    }
    bool empty(const remote_player_skin& skin) {
        return skin.skins.empty() && !skin.agent_ct && !skin.agent_t && !skin.music_kit_id;
    }
    nlohmann::json request(const nlohmann::json& payload) {
        const auto response = detail::http_post_json(payload.dump());
        if (!response.ok()) {
            diag::writef(diag::level::warning, "[skin-sync] request failed http=%lu win32=%lu", response.status, response.error);
            return {};
        }
        auto reply = nlohmann::json::parse(response.body, nullptr, false);
        if (!reply.is_object() || !reply.contains("success") || !reply["success"].is_boolean() || !reply["success"].get<bool>()) return {};
        return reply;
    }
}

void skin_sync::initialize() {
    std::lock_guard lock(m_worker_mutex);
    if (m_stopping || lifecycle::is_unloading() || m_initialized.exchange(true)) return;
    m_running = true;
    m_push_pending = true;
    m_worker_handle = CreateThread(nullptr, 0, [](LPVOID data) -> DWORD {
        auto* self = static_cast<skin_sync*>(data);
        for (int i = 0; i < 30 && self->m_running.load(); ++i) Sleep(100);
        if (self->m_running.load()) self->worker_loop();
        return 0;
    }, this, 0, nullptr);
    if (!m_worker_handle) {
        m_running = false;
        m_initialized = false;
        diag::writef(diag::level::error, "[skin-sync] CreateThread failed win32=%lu", GetLastError());
    }
}

bool skin_sync::shutdown() {
    std::lock_guard lock(m_worker_mutex);
    m_stopping = true;
    m_running = false;
    if (!m_worker_handle) return true;
    const auto status = WaitForSingleObject(m_worker_handle, 30000);
    if (status != WAIT_OBJECT_0) {
        diag::writef(diag::level::error, "[skin-sync] worker not stopped status=%lu; unload must be cancelled", status);
        return false;
    }
    CloseHandle(m_worker_handle);
    m_worker_handle = nullptr;
    return true;
}

bool skin_sync::is_enabled() const { return m_enabled.load(); }
void skin_sync::trigger_push() { m_push_pending = true; }

void skin_sync::on_sync_toggled() {
    // Only the render/menu thread reads mutable UI settings.
    {
        std::unique_lock lock(m_mutex);
        m_enabled = settings::g_changer.sync_enabled.value;
        ++m_epoch;
        m_cache.clear();
        m_cheat_users.clear();
        m_bot_preview = {};
        m_local_payload.clear();
    }
    {
        std::lock_guard lock(m_query_mutex);
        m_pending_query_ids.clear();
    }
    auto sid = steam::user::get_steam_id();
    if (sid < steam_base) sid = m_last_local_steam_id.load();
    if (sid >= steam_base) {
        set_local_steam_id(sid);
        capture_local_snapshot(sid);
    }
    m_push_pending = true;
    m_schedule_reset = true;
}

void skin_sync::set_local_steam_id(std::uint64_t sid) {
    if (sid < steam_base) return;
    std::unique_lock lock(m_mutex);
    if (m_last_local_steam_id.exchange(sid) == sid) return;
    ++m_epoch;
    m_local_team = 0;
    m_cache.clear();
    m_cheat_users.clear();
    m_bot_preview = {};
    m_local_payload.clear();
    m_push_pending = true;
    m_schedule_reset = true;
}

std::uint64_t skin_sync::resolve_local_steam_id() const {
    const auto ctrl = systems::g_local.get().controller;
    const auto offset = SCHEMA("CBasePlayerController", "m_steamID"_hash);
    const auto sid = ctrl && offset ? memory::safe_read<std::uint64_t>(ctrl + offset).value_or(0) : 0;
    if (sid >= steam_base) return sid;
    const auto steam_id = steam::user::get_steam_id();
    return steam_id >= steam_base ? steam_id : m_last_local_steam_id.load();
}

void skin_sync::capture_local_snapshot(std::uint64_t sid) {
    remote_player_skin snapshot{};
    if (is_enabled()) {
        const auto team = m_local_team.load();
        if (team == 2 || team == 3) snapshot.skins = settings::g_changer.skins.for_team(team);
        else {
            snapshot.skins = settings::g_changer.skins.for_team(3);
            for (const auto& [def, skin] : settings::g_changer.skins.for_team(2)) snapshot.skins.try_emplace(def, skin);
        }
        snapshot.music_kit_id = settings::g_changer.music.id;
        const auto agent = [](std::int16_t id, int custom, int team) -> std::int16_t {
            const auto& entries = settings::g_changer.custom_agents.entries;
            if (custom >= 0 && custom < static_cast<int>(entries.size())) {
                const auto& entry = entries[custom];
                if (!entry.model_path.empty() && (entry.team == 0 || entry.team == team)) return 0;
            }
            const auto def = id > 0 ? g_econ_item_system.find_def(id) : nullptr;
            return def && def->category == econ_item_system::item_category::agent && !def->model_player.empty() &&
                (def->team() == 0 || def->team() == team) ? id : 0;
        };
        snapshot.agent_ct = agent(settings::g_changer.agents.ct_def, settings::g_changer.custom_agents.selected_ct, 3);
        snapshot.agent_t = agent(settings::g_changer.agents.t_def, settings::g_changer.custom_agents.selected_t, 2);
    }
    snapshot.last_updated = std::chrono::steady_clock::now();
    auto skins = nlohmann::json::object();
    for (const auto& [def, skin] : snapshot.skins) {
        skins[std::to_string(def)] = {{"p", skin.paint_kit_id}, {"w", skin.wear}, {"s", skin.seed},
            {"t", skin.stattrak}, {"c", skin.stattrak_count}, {"n", ""}};
    }
    const nlohmann::json payload = {{"action", "skin_sync_push"}, {"steam_id", std::to_string(sid)},
        {"skin_data", skins}, {"music_kit_id", snapshot.music_kit_id}, {"agent_ct", snapshot.agent_ct}, {"agent_t", snapshot.agent_t}};
    const auto encoded = payload.dump();
    std::unique_lock lock(m_mutex);
    if (sid != m_last_local_steam_id.load()) return;
    m_bot_preview = std::move(snapshot);
    if (encoded != m_local_payload || sid != m_payload_steam_id) {
        m_local_payload = encoded;
        m_payload_steam_id = sid;
        m_push_pending = true;
    }
}

void skin_sync::on_present() {
    if (lifecycle::is_unloading()) return;
    if (settings::g_changer.sync_enabled.value != is_enabled()) on_sync_toggled();
    const bool active = systems::g_local.get().controller != 0;
    if (m_match_active.exchange(active) != active) {
        {
            std::unique_lock lock(m_mutex);
            ++m_epoch;
            m_cache.clear();
            m_cheat_users.clear();
        }
        {
            std::lock_guard lock(m_query_mutex);
            m_pending_query_ids.clear();
        }
        if (!active) m_local_team = 0;
        m_schedule_reset = true;
        m_push_pending = true;
    }
    if (active && !m_initialized.load()) initialize();
    if (!m_initialized.load() || !m_running.load()) return;
    const auto now = std::chrono::steady_clock::now();
    if (now - m_last_snapshot_time < std::chrono::milliseconds(250)) return;
    m_last_snapshot_time = now;
    auto sid = steam::user::get_steam_id();
    if (sid < steam_base) sid = m_last_local_steam_id.load();
    if (sid < steam_base) return;
    set_local_steam_id(sid);
    try { capture_local_snapshot(sid); }
    catch (const std::exception&) { diag::write(diag::level::warning, "[skin-sync] snapshot serialization failed"); }
}

void skin_sync::on_frame_stage_notify() {
    if (lifecycle::is_unloading() || !is_enabled()) return;
    const auto local = systems::g_local.get();
    if (!local.controller) return;
    const auto sid = resolve_local_steam_id();
    if (sid >= steam_base) set_local_steam_id(sid);
    const auto team_offset = SCHEMA("C_BaseEntity", "m_iTeamNum"_hash);
    if (team_offset) {
        const auto team = memory::safe_read<std::uint8_t>(local.controller + team_offset).value_or(0);
        m_local_team = cosmetic_config::retain_team(m_local_team.load(), team);
    }
    const auto now = std::chrono::steady_clock::now();
    if (m_query_controller != local.controller) { m_query_controller = local.controller; m_next_query_time = {}; }
    if (now < m_next_query_time) return;
    m_next_query_time = now + std::chrono::milliseconds(250);
    std::vector<std::uint64_t> ids;
    const auto append = [&](std::uint64_t id) {
        if (id >= steam_base && std::find(ids.begin(), ids.end(), id) == ids.end()) ids.push_back(id);
    };
    append(sid);
    const auto offset = SCHEMA("CBasePlayerController", "m_steamID"_hash);
    if (!offset) return;
    for (const auto& player : systems::g_entities.get_by_type(systems::entities::type::player)) {
        if (preview_scene::is_controller(player.ptr)) append(memory::safe_read<std::uint64_t>(player.ptr + offset).value_or(0));
    }
    for (const auto& preview : preview_scene::players) append(preview.steam_id);
    std::lock_guard lock(m_query_mutex);
    for (const auto pending : m_pending_query_ids) append(pending);
    m_pending_query_ids = std::move(ids);
}

std::optional<remote_player_skin> skin_sync::get_remote_skin(std::uint64_t sid) const {
    std::shared_lock lock(m_mutex);
    if (!is_enabled() || !m_match_active.load()) return std::nullopt;
    if (sid < steam_base) {
        if (!m_bot_sync_test.load()) return std::nullopt;
        const auto own = m_cache.find(m_last_local_steam_id.load());
        if (own != m_cache.end() && fresh(own->second) && !empty(own->second)) return own->second;
        return fresh(m_bot_preview) && !empty(m_bot_preview) ? std::optional<remote_player_skin>{m_bot_preview} : std::nullopt;
    }
    const auto found = m_cache.find(sid);
    if (found == m_cache.end() || !fresh(found->second) || empty(found->second)) return std::nullopt;
    return found->second;
}

bool skin_sync::is_cheat_user(std::uint64_t sid) const {
    if (!is_enabled()) return false;
    if (sid >= steam_base && sid == m_last_local_steam_id.load()) return true;
    if (sid < steam_base) return m_bot_sync_test.load();
    return get_remote_skin(sid).has_value();
}
bool skin_sync::should_show_indicator(std::uint64_t sid) const {
    if (!is_enabled()) return false;
    if (sid < steam_base) return m_bot_sync_test.load();
    const bool user = is_cheat_user(sid);
    return m_test_inversion.load() ? !user : user;
}
int skin_sync::get_remote_music_kit(std::uint64_t sid) const {
    const auto profile = get_remote_skin(sid);
    return profile ? profile->music_kit_id : 0;
}

void skin_sync::worker_loop() {
    auto next_push = std::chrono::steady_clock::now();
    unsigned failures = 0;
    while (m_running.load()) {
        if (m_schedule_reset.exchange(false)) {
            m_last_push_time = {}; m_last_pull_time = {}; m_last_users_time = {};
            next_push = std::chrono::steady_clock::now(); failures = 0;
        }
        const auto now = std::chrono::steady_clock::now();
        try {
            const bool heartbeat = is_enabled() && m_match_active.load() && now - m_last_push_time >= std::chrono::seconds(10);
            if (now >= next_push && (m_push_pending.load() || heartbeat)) {
                if (perform_push()) {
                    failures = 0; m_last_push_time = std::chrono::steady_clock::now();
                    next_push = m_last_push_time + std::chrono::seconds(1);
                } else {
                    if (failures < 5) ++failures;
                    next_push = std::chrono::steady_clock::now() + std::chrono::seconds(1u << failures);
                }
            }
            if (m_running.load() && is_enabled() && m_match_active.load() && now - m_last_pull_time >= std::chrono::seconds(2)) {
                perform_pull(); m_last_pull_time = std::chrono::steady_clock::now();
            }
            if (m_running.load() && is_enabled() && m_match_active.load() && now - m_last_users_time >= std::chrono::seconds(3)) {
                perform_users_update(); m_last_users_time = std::chrono::steady_clock::now();
            }
        } catch (const std::exception&) { diag::write(diag::level::warning, "[skin-sync] worker request failed"); }
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}

bool skin_sync::perform_push() {
    std::string payload;
    std::uint64_t sid{}, epoch{};
    {
        std::shared_lock lock(m_mutex);
        payload = m_local_payload; sid = m_payload_steam_id; epoch = m_epoch;
    }
    if (payload.empty()) return false;
    const auto response = detail::http_post_json(payload);
    if (!response.ok()) return false;
    const auto reply = nlohmann::json::parse(response.body, nullptr, false);
    if (!reply.is_object() || !reply.contains("success") || !reply["success"].is_boolean() || !reply["success"].get<bool>() ||
        !reply.contains("steam_id") || !reply["steam_id"].is_string() || reply["steam_id"].get<std::string>() != std::to_string(sid)) return false;
    std::unique_lock lock(m_mutex);
    if (!m_running.load() || epoch != m_epoch) return false;
    if (payload == m_local_payload && sid == m_payload_steam_id) m_push_pending = false;
    return true;
}

void skin_sync::perform_pull() {
    std::uint64_t epoch{};
    {
        std::shared_lock lock(m_mutex);
        if (!is_enabled() || !m_match_active.load()) return;
        epoch = m_epoch;
    }
    std::vector<std::uint64_t> ids;
    {
        std::lock_guard lock(m_query_mutex);
        const auto count = std::min<std::size_t>(m_pending_query_ids.size(), 256);
        ids.assign(m_pending_query_ids.begin(), m_pending_query_ids.begin() + count);
        m_pending_query_ids.erase(m_pending_query_ids.begin(), m_pending_query_ids.begin() + count);
    }
    if (ids.empty()) return;
    auto array = nlohmann::json::array();
    for (const auto id : ids) array.push_back(std::to_string(id));
    const auto reply = request({{"action", "skin_sync_pull"}, {"steam_ids", array}});
    if (!reply.is_object() || !reply.contains("users")) return;
    const auto& users = reply["users"];
    if (!users.is_object() && !(users.is_array() && users.empty())) return;
    std::unordered_map<std::uint64_t, remote_player_skin> updates;
    for (const auto id : ids) {
        const auto key = std::to_string(id);
        if (!users.contains(key)) continue;
        const auto& data = users[key];
        if (!data.is_object()) continue;
        remote_player_skin skin;
        skin.music_kit_id = cosmetic_config::field(data, "music_kit_id", 0, 65534);
        skin.agent_ct = static_cast<std::int16_t>(cosmetic_config::field(data, "agent_ct", 0, 32767));
        skin.agent_t = static_cast<std::int16_t>(cosmetic_config::field(data, "agent_t", 0, 32767));
        if (data.contains("skins") && data["skins"].is_object()) skin.skins = settings::changer::skin_map_field::decode_map(data["skins"]);
        skin.last_updated = std::chrono::steady_clock::now();
        updates.emplace(id, std::move(skin));
    }
    std::unique_lock lock(m_mutex);
    if (!m_running.load() || !is_enabled() || !m_match_active.load() || epoch != m_epoch) return;
    std::erase_if(m_cache, [](const auto& item) { return !fresh(item.second); });
    for (const auto id : ids) {
        if (!users.contains(std::to_string(id))) { m_cache.erase(id); m_cheat_users.erase(id); }
    }
    for (auto& [id, skin] : updates) {
        if (empty(skin)) { m_cache.erase(id); m_cheat_users.erase(id); }
        else { m_cache.insert_or_assign(id, std::move(skin)); m_cheat_users.insert(id); }
    }
}

void skin_sync::perform_users_update() {
    std::uint64_t epoch{};
    {
        std::shared_lock lock(m_mutex);
        if (!is_enabled() || !m_match_active.load()) return;
        epoch = m_epoch;
    }
    const auto reply = request({{"action", "skin_sync_users"}});
    if (!reply.is_object() || !reply.contains("users") || !reply["users"].is_array()) return;
    std::unordered_set<std::uint64_t> users;
    for (const auto& value : reply["users"]) {
        if (!value.is_string()) continue;
        try {
            const auto text = value.get<std::string>();
            const auto sid = std::stoull(text);
            if (sid >= steam_base && std::to_string(sid) == text) users.insert(sid);
        } catch (const std::exception&) {}
    }
    std::unique_lock lock(m_mutex);
    if (!m_running.load() || !is_enabled() || !m_match_active.load() || epoch != m_epoch) return;
    m_cheat_users = std::move(users);
    // Only visible match/preview IDs are queued by the game thread. Do not let
    // global discovery grow the HTTP backlog or displace current players.
}
} // namespace features::changer
