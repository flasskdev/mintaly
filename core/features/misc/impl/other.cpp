#include <pch/pch.hpp>
#include <algorithm>
#include <bit>
#include <chrono>
#include <utilities/nickname_animation.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <utilities/logging/logging.hpp>
#include <utilities/steam/steam.hpp>
#include <utilities/random/random.hpp>
#include <core/settings.hpp>
#include <core/features/features.hpp>
#include <core/features/changer/cosmetic_attributes.hpp>
#include <external/config.hpp>
#include <protection/game_addresses.hpp>

namespace features::misc {
        namespace {

                [[nodiscard]] std::string controller_name(std::uintptr_t controller)
                {
                        if (!controller || controller < 0x10000)
                        {
                                return {};
                        }

                        const auto offset = SCHEMA("CCSPlayerController", "m_sSanitizedPlayerName"_hash);
                        if (!offset)
                                return {};
                        const auto name_ptr = memory::safe_read<std::uintptr_t>(
                                controller + offset).value_or(0);
                        return name_ptr >= 0x10000 ? memory::read_string(name_ptr, 127) : std::string{};
                }

                [[nodiscard]] std::string random_match_player_name(const std::string& exclude = {})
                {
                        std::vector<std::string> names;
                        const auto local = systems::g_local.get();
                        for (const auto& player : systems::g_entities.get_by_type(systems::entities::type::player))
                        {
                                if (!player.ptr || player.ptr == local.controller)
                                        continue;

                                if ( player.ptr < 0x10000 )
                                        continue;
                                const auto offset = SCHEMA("CCSPlayerController", "m_sSanitizedPlayerName"_hash);
                                if (!offset)
                                        continue;
                                const auto name_ptr = memory::safe_read<std::uintptr_t>(
                                        player.ptr + offset).value_or(0);
                                if (name_ptr < 0x10000)
                                        continue;

                                auto pname = memory::read_string(name_ptr, 127);
                                if (pname.empty() || pname == "x")
                                        continue;
                                if (!exclude.empty() && pname == exclude)
                                        continue;

                                names.push_back(std::move(pname));
                        }

                        if (names.empty())
                        {
                                return {};
                        }

                        return names[static_cast<std::size_t>(random::integer(0, static_cast<int>(names.size()) - 1))];
                }

                void repair_native_radar_hud()
                {
                        // Level transitions can recreate the HUD and restore
                        // its ConVars, so verify periodically instead of
                        // consuming a process-wide one-shot during loading.
                        static auto next_check = std::chrono::steady_clock::time_point{};
                        static auto next_report = std::chrono::steady_clock::time_point{};
                        const auto now = std::chrono::steady_clock::now();
                        if ( now < next_check )
                                return;
                        next_check = now + std::chrono::seconds( 1 );

                        const auto draw_hud = CONVAR("cl_drawhud");
                        const auto force_radar = CONVAR("cl_drawhud_force_radar");
                        const auto death_notices_only = CONVAR("cl_draw_only_deathnotices");
                        const auto hud_radar_scale = CONVAR("cl_hud_radar_scale");
                        const auto radar_map_additive = CONVAR("cl_hud_radar_map_additive");
                        const auto radar_background_alpha = CONVAR("cl_hud_radar_background_alpha");
                        const auto radar_scale = CONVAR("cl_radar_scale");
                        const auto radar_icon_scale = CONVAR("cl_radar_icon_scale_min");
                        const auto radar_centered = CONVAR("cl_radar_always_centered");

                        // ConVars may not exist during the first network-stage
                        // callback after injection. Keep trying until the HUD
                        // has initialized instead of permanently consuming the
                        // one-shot repair before there is anything to repair.
                        if ( !hud_radar_scale || !radar_scale || !radar_icon_scale )
                                return;

                        bool changed = false;
                        if ( draw_hud && draw_hud->m_value.i1 == 0 )
                        {
                                draw_hud->m_value.i1 = true;
                                ++draw_hud->m_change_count;
                                changed = true;
                        }
                        if ( force_radar && force_radar->m_value.i32 < 0 )
                        {
                                // -1 explicitly suppresses the native radar,
                                // independently from cl_drawhud.
                                force_radar->m_value.i32 = 0;
                                ++force_radar->m_change_count;
                                changed = true;
                        }
                        if ( death_notices_only && death_notices_only->m_value.i1 != 0 )
                        {
                                death_notices_only->m_value.i1 = 0;
                                ++death_notices_only->m_change_count;
                                changed = true;
                        }
                        if ( hud_radar_scale && ( hud_radar_scale->m_value.fl <= 0.0f || hud_radar_scale->m_value.fl > 5.0f ) )
                        {
                                hud_radar_scale->m_value.fl = 1.0f;
                                ++hud_radar_scale->m_change_count;
                                changed = true;
                        }
                        if ( radar_scale && ( radar_scale->m_value.fl <= 0.0f || radar_scale->m_value.fl > 1.0f ) )
                        {
                                radar_scale->m_value.fl = 0.7f;
                                ++radar_scale->m_change_count;
                                changed = true;
                        }
                        if ( radar_icon_scale && ( radar_icon_scale->m_value.fl <= 0.0f || radar_icon_scale->m_value.fl > 2.0f ) )
                        {
                                radar_icon_scale->m_value.fl = 0.6f;
                                ++radar_icon_scale->m_change_count;
                                changed = true;
                        }
                        // Restore the stock overview-map blend mode if a profile
                        // or game update disabled it.
                        if ( radar_map_additive && !radar_map_additive->m_value.i1 )
                        {
                                radar_map_additive->m_value.i1 = true;
                                ++radar_map_additive->m_change_count;
                                changed = true;
                        }
                        if ( radar_background_alpha && ( radar_background_alpha->m_value.fl < 0.0f || radar_background_alpha->m_value.fl > 1.0f ) )
                        {
                                radar_background_alpha->m_value.fl = 0.627f;
                                ++radar_background_alpha->m_change_count;
                                changed = true;
                        }
                        if ( radar_centered && !radar_centered->m_value.i1 )
                        {
                                // Restore the stock behavior and undo the earlier
                                // forced scrolling mode that moved the player icon
                                // away from the center of the radar.
                                radar_centered->m_value.i1 = true;
                                ++radar_centered->m_change_count;
                                changed = true;
                        }

                        if ( changed && now >= next_report )
                        {
                                next_report = now + std::chrono::seconds( 30 );
                                diag::writef( diag::level::info,
                                        "[radar-hud] drawhud=%d force_radar=%d death_only=%d hud_scale=%.3f map_additive=%d bg_alpha=%.3f radar_scale=%.3f icon_scale=%.3f centered=%d repaired=%d",
                                        draw_hud ? draw_hud->m_value.i1 : -1,
                                        force_radar ? force_radar->m_value.i32 : -2,
                                        death_notices_only ? death_notices_only->m_value.i1 : -1,
                                        hud_radar_scale ? hud_radar_scale->m_value.fl : -1.0f,
                                        radar_map_additive ? radar_map_additive->m_value.i1 : -1,
                                        radar_background_alpha ? radar_background_alpha->m_value.fl : -1.0f,
                                        radar_scale ? radar_scale->m_value.fl : -1.0f,
                                        radar_icon_scale ? radar_icon_scale->m_value.fl : -1.0f,
                                        radar_centered ? radar_centered->m_value.i1 : -1,
                                        static_cast<int>( changed ) );
                        }
                }

                [[nodiscard]] std::string make_random_nickname()
                {
                        static constexpr const char* const k_parts[][ 16 ]{
                                { "Shadow", "Ghost", "Storm", "Frost", "Night", "Savage", "Crazy", "Pro", "Top", "Killer", "Silent", "Dark", "Blaze", "Fast", "Lucky", "Sneaky" },
                                { "Player", "Shooter", "Sniper", "Rusher", "Clutch", "Legend", "Striker", "Aimer", "Fragger", "Hunter", "Runner", "Viper", "Wolf", "Fox", "Eagle", "Panda" }
                        };
                        const auto part_a = k_parts[ 0 ][ random::integer( 0, 15 ) ];
                        const auto part_b = k_parts[ 1 ][ random::integer( 0, 15 ) ];
                        const auto suffix = random::integer( 10, 999 );
                        return std::format( "{}{}{}", part_a, part_b, suffix );
                }

                [[nodiscard]] std::string get_steam_nickname(std::uintptr_t controller = 0)
                {
                        if (auto persona = steam::friends::get_persona_name(); !persona.empty())
                        {
                                if (persona != "x")
                                {
                                        return persona;
                                }
                        }

                        if (controller)
                        {
                                const auto offset = SCHEMA("CCSPlayerController", "m_sSanitizedPlayerName"_hash);
                                const auto name_ptr = offset
                                        ? memory::safe_read<std::uintptr_t>(controller + offset).value_or(0) : 0;
                                if (name_ptr >= 0x10000)
                                {
                                        auto name = memory::read_string(name_ptr, 127);
                                        if (!name.empty() && name != "x")
                                        {
                                                return name;
                                        }
                                }

                                const auto raw_offset = SCHEMA("CBasePlayerController", "m_iszPlayerName"_hash);
                                if (raw_offset)
                                {
                                        auto raw_name = memory::read_string(controller + raw_offset, 127);
                                        if (!raw_name.empty() && raw_name != "x")
                                        {
                                                return raw_name;
                                        }
                                }
                        }

                        return {};
                }

                void submit_name_change(const std::string& display_name)
                {
                        if (display_name.empty() || display_name.size() > 127)
                        {
                                return;
                        }

                        const auto engine = addresses::globals::source2engine_to_client;
                        const auto command_fn = PATTERN(patterns::engine_client_cmd);
                        if (!engine || engine < 0x10000 || !command_fn)
                                return;

                        std::string sanitized = display_name;
                        std::erase(sanitized, '"');
                        std::erase(sanitized, '\n');
                        std::erase(sanitized, '\r');
                        std::erase(sanitized, ';');

                        other::s_display_name = sanitized;
                        other::s_name_change_pending = true;

                        const auto cmd = std::format("setinfo name \"{}\"", sanitized);
                        memory::safe_call<void>(command_fn, engine, 0, cmd.c_str(), 0x7ffef001);
                }

                inline bool is_local_player( std::uintptr_t ent, const systems::local::snapshot& local )
                {
                        if ( !ent || !local.is_valid( ) ) return false;
                        if ( ent == local.controller || ent == local.pawn ) return true;

                        const auto ctrl_h = memory::read<std::uint32_t>( ent + SCHEMA( "C_BasePlayerPawn", "m_hController"_hash ) );
                        if ( ctrl_h && ctrl_h != 0xFFFFFFFF && systems::g_entities.lookup( ctrl_h ) == local.controller ) return true;

                        const auto pawn_h = memory::read<std::uint32_t>( ent + SCHEMA( "CBasePlayerController", "m_hPawn"_hash ) );
                        if ( pawn_h && pawn_h != 0xFFFFFFFF && systems::g_entities.lookup( pawn_h ) == local.pawn ) return true;

                        return false;
                }

                inline std::uintptr_t resolve_pawn( std::uintptr_t ent )
                {
                        if ( !ent ) return 0;
                        const auto ctrl_h = memory::read<std::uint32_t>( ent + SCHEMA( "C_BasePlayerPawn", "m_hController"_hash ) );
                        if ( ctrl_h && ctrl_h != 0xFFFFFFFF ) return ent;

                        const auto pawn_h = memory::read<std::uint32_t>( ent + SCHEMA( "CBasePlayerController", "m_hPawn"_hash ) );
                        if ( pawn_h && pawn_h != 0xFFFFFFFF )
                        {
                                const auto pawn = systems::g_entities.lookup( pawn_h );
                                if ( pawn ) return pawn;
                        }
                        return ent;
                }


        } // namespace

        void other::on_round_start()
        {
                if (!config::registry::flush_pending_save())
                        diag::write(diag::level::warning, "StatTrak: deferred config save failed");
                features::misc::g_vote_logs.reset();
                this->do_autobuy();
        }

        void other::on_player_death(std::uintptr_t event)
        {
                if (!event)
                {
                        return;
                }

                auto attacker = systems::events::get_controller(reinterpret_cast<void*>(event), "attacker");
                if (!attacker)
                {
                        attacker = systems::events::get_pawn(reinterpret_cast<void*>(event), "attacker");
                }
                auto victim = systems::events::get_controller(reinterpret_cast<void*>(event), "userid");
                if (!victim)
                {
                        victim = systems::events::get_pawn(reinterpret_cast<void*>(event), "userid");
                }

                const auto local = systems::g_local.get();
                if (!local.is_valid() || !attacker || !is_local_player(attacker, local) || is_local_player(victim, local))
                {
                        return;
                }

                auto victim_pawn = systems::events::get_pawn(reinterpret_cast<void*>(event), "userid");
                if (!victim_pawn && victim)
                {
                        victim_pawn = resolve_pawn(victim);
                }

                if (!victim_pawn)
                {
                        return;
                }

                const auto victim_team = memory::read<std::uint8_t>(victim_pawn + SCHEMA("C_BaseEntity", "m_iTeamNum"_hash));
                if (!local.is_this_other_team(victim_team))
                {
                        return;
                }

                const auto weapon_services = memory::read<std::uintptr_t>(local.pawn + SCHEMA("C_BasePlayerPawn", "m_pWeaponServices"_hash));
                if (!weapon_services)
                {
                        return;
                }

                const auto active_handle = memory::read<std::uint32_t>(weapon_services + SCHEMA("CPlayer_WeaponServices", "m_hActiveWeapon"_hash));
                const auto active_weapon = systems::g_entities.lookup(active_handle);
                if (!active_weapon)
                {
                        return;
                }

                const auto iv = active_weapon + SCHEMA("C_EconEntity", "m_AttributeManager"_hash) + SCHEMA("C_AttributeContainer", "m_Item"_hash);
                const auto def_index = memory::read<std::uint16_t>(iv + SCHEMA("C_EconItemView", "m_iItemDefinitionIndex"_hash));
                const auto def = changer::g_econ_item_system.find_def(static_cast<std::int16_t>(def_index));
                if (!def)
                {
                        return;
                }

                std::lock_guard config_lock(config::registry::g_io_mutex);
                const auto team_offset = SCHEMA("C_BaseEntity", "m_iTeamNum"_hash);
                const int kill_team = team_offset
                        ? memory::safe_read<std::uint8_t>(local.controller + team_offset).value_or(0) : local.team;
                if (kill_team != 2 && kill_team != 3) return;
                auto& active_skins = settings::g_changer.skins.for_team(kill_team);
                settings::changer::applied_skin* target_skin{ nullptr };
                if (def->category == changer::econ_item_system::item_category::gun)
                {
                        const auto it = active_skins.find(def_index);
                        if (it != active_skins.end())
                        {
                                target_skin = &it->second;
                        }
                }
                else if (def->category == changer::econ_item_system::item_category::knife)
                {
                        const auto it = active_skins.find(def_index);
                        if (it != active_skins.end())
                        {
                                target_skin = &it->second;
                        }
                        else
                        {
                                for (auto& [k_def, k_skin] : active_skins)
                                {
                                        const auto kdef = changer::g_econ_item_system.find_def(k_def);
                                        if (kdef && kdef->category == changer::econ_item_system::item_category::knife)
                                        {
                                                target_skin = &k_skin;
                                                break;
                                        }
                                }
                        }
                }

                if (target_skin && target_skin->stattrak)
                {
                        if (target_skin->stattrak_count < (std::numeric_limits<int>::max)())
                                ++target_skin->stattrak_count;

                        memory::write<int>(active_weapon + SCHEMA("C_EconEntity", "m_nFallbackStatTrak"_hash), target_skin->stattrak_count);

                        if (changer::cosmetic_attributes::available())
                        {
                                changer::cosmetic_attributes::sanitize(iv);
                                const auto set = PATTERN(patterns::econ_item_view_set_attribute);
                                const auto count_val = std::bit_cast<float>(static_cast<std::int32_t>(target_skin->stattrak_count));
                                memory::safe_call<void>(set, iv, "kill eater", count_val);
                        }

                        // Per-item skin comparisons already observe the new count.
                        // Invalidating every weapon here caused unnecessary rebuilds.
                        changer::g_skin_sync.trigger_push();
                        config::registry::request_save_active();
                }

                if (settings::g_misc.m_kill_say.enabled.value && !settings::g_misc.m_kill_say.message.value.empty())
                {
                        std::string text = settings::g_misc.m_kill_say.message.value;
                        std::replace(text.begin(), text.end(), '\n', ' ');
                        std::replace(text.begin(), text.end(), '\r', ' ');
                        std::replace(text.begin(), text.end(), '"', '\'');
                        std::replace(text.begin(), text.end(), ';', ' ');
                        const auto cmd = std::format("say \"{}\"", text);
                        const auto engine = addresses::globals::source2engine_to_client;
                        const auto command_fn = PATTERN(patterns::engine_client_cmd);
                        if ( engine && engine >= 0x10000 && command_fn )
                                memory::safe_call<void>(command_fn, engine, 0, cmd.c_str(), 0x7ffef001);
                }
        }

        void other::on_frame_stage_notify()
        {
                if ( settings::g_misc.vote_kick_self.value )
                {
                        settings::g_misc.vote_kick_self.value = false;
                        this->vote_kick_self( );
                }

                this->do_player_alpha_changing();
                this->do_name_changing();
                this->do_chat_spam();
        }

        void other::do_reveal_radar() const
        {
                const auto report = []( const char* reason, std::uint32_t state = 0,
                        std::uint32_t spotted = 0, std::uint32_t mask = 0,
                        std::uint32_t team = 0, std::size_t controllers = 0,
                        std::size_t alive = 0, std::size_t candidates = 0,
                        std::size_t written = 0 )
                {
                        static std::string_view last_reason{};
                        // Disabled radar is the normal idle state, so it should
                        // not emit one diagnostic per second from the frame hook.
                        if ( !settings::g_misc.reveal_radar.value )
                        {
                                last_reason = "setting-disabled";
                                return;
                        }
                        // Report state transitions only. Counts can change every
                        // frame and are not useful as a reason to repeat the same
                        // diagnostic continuously.
                        if ( last_reason == reason ) return;
                        last_reason = reason;
                        diag::writef( diag::level::info,
                                "[radar] reason=%s enabled=%d local=%d controllers=%zu alive=%zu candidates=%zu written=%zu offsets=state:%u spotted:%u mask:%u team:%u",
                                reason, static_cast<int>( settings::g_misc.reveal_radar.value ),
                                static_cast<int>( systems::g_local.get().is_valid() ), controllers,
                                alive, candidates, written, state, spotted, mask, team );
                };

                const auto local = systems::g_local.get();

                // Only repair HUD when reveal radar is enabled.
                // When disabled, keep the stock minimap usable.
                if (settings::g_misc.reveal_radar.value)
                {
                        repair_native_radar_hud();
                }

                if (!settings::g_misc.reveal_radar.value)
                {
                        report( "setting-disabled" ); // records the state transition without writing a log line
                        return;
                }
                if (!local.is_valid())
                {
                        report( "local-invalid" );
                        return;
                }

                // This field can be declared on the pawn base after a game update.
                // Never write with a zero/unresolved schema offset: pawn + 0 is
                // the object header/vtable and corrupting it breaks the radar
                // and usually crashes client.dll during rendering.
                auto spotted_state_offset = SCHEMA("C_CSPlayerPawn", "m_entitySpottedState"_hash);
                if (!spotted_state_offset)
                {
                        spotted_state_offset = SCHEMA("C_CSPlayerPawnBase", "m_entitySpottedState"_hash);
                }
                const auto spotted_offset = SCHEMA("EntitySpottedState_t", "m_bSpotted"_hash);
                const auto spotted_mask_offset = SCHEMA("EntitySpottedState_t", "m_bSpottedByMask"_hash);

                if (!spotted_state_offset || !spotted_offset)
                {
                        static std::atomic_bool warned{};
                        if (!warned.exchange(true, std::memory_order_relaxed))
                        {
                                diag::write(diag::level::warning,
                                        "reveal radar disabled: m_entitySpottedState/m_bSpotted schema unresolved");
                        }
                        report( "spotted-schema-missing", spotted_state_offset,
                                spotted_offset, spotted_mask_offset );
                        return;
                }

                const auto alive_offset = SCHEMA( "CCSPlayerController", "m_bPawnIsAlive"_hash );
                const auto player_pawn_handle_offset = SCHEMA( "CCSPlayerController", "m_hPlayerPawn"_hash );
                const auto pawn_handle_offset = SCHEMA( "CBasePlayerController", "m_hPawn"_hash );
                const auto team_offset = SCHEMA( "C_BaseEntity", "m_iTeamNum"_hash );
                if ( !alive_offset || ( !player_pawn_handle_offset && !pawn_handle_offset ) )
                {
                        report( "controller-schema-missing", spotted_state_offset,
                                spotted_offset, spotted_mask_offset, team_offset );
                        return;
                }

                std::size_t controllers = 0;
                std::size_t alive_players = 0;
                std::size_t candidate_pawns = 0;
                std::size_t spotted_players = 0;
                for (const auto& player : systems::g_entities.get_by_type(systems::entities::type::player))
                {
                        ++controllers;
                        const auto controller = player.ptr;
                        if (!controller || !memory::safe_read<bool>( controller + alive_offset ).value_or( false ))
                        {
                                continue;
                        }
                        ++alive_players;

                        auto pawn_handle = player_pawn_handle_offset
                                ? memory::safe_read<std::uint32_t>( controller + player_pawn_handle_offset ).value_or( 0 )
                                : 0;
                        if ( ( !pawn_handle || pawn_handle == 0xffffffffu ) && pawn_handle_offset )
                        {
                                pawn_handle = memory::safe_read<std::uint32_t>( controller + pawn_handle_offset ).value_or( 0 );
                        }
                        const auto pawn = systems::g_entities.lookup(pawn_handle);
                        if (!pawn || pawn == local.view_pawn())
                        {
                                continue;
                        }
                        ++candidate_pawns;

                        // If the team schema is unavailable or the field read
                        // fails, do not silently discard the player. The
                        // previous hard filter made the feature a no-op on
                        // builds where m_iTeamNum moved to a derived class.
                        if ( team_offset )
                        {
                                const auto team = memory::safe_read<std::uint8_t>( pawn + team_offset ).value_or( 0 );
                                if ( ( team == 2 || team == 3 ) && !local.is_this_other_team(team) )
                                {
                                        continue;
                                }
                        }

                        if ( memory::safe_write<bool>(pawn + spotted_state_offset + spotted_offset, true) )
                        {
                                ++spotted_players;
                        }
                        if ( spotted_mask_offset )
                        {
                                // The mask is two 32-bit controller masks in
                                // EntitySpottedState_t. Set both halves so the
                                // marker is visible to every HUD radar owner,
                                // not only the local controller.
                                memory::safe_write<std::uint32_t>( pawn + spotted_state_offset + spotted_mask_offset, 0xffffffffu );
                                memory::safe_write<std::uint32_t>( pawn + spotted_state_offset + spotted_mask_offset + sizeof( std::uint32_t ), 0xffffffffu );
                        }
                }

                report( "updated", spotted_state_offset, spotted_offset,
                        spotted_mask_offset, team_offset, controllers, alive_players,
                        candidate_pawns, spotted_players );
        }

        void other::do_autobuy() const
        {
                if (!settings::g_misc.m_autobuy.enabled)
                {
                        return;
                }

                std::string cmd{};

                switch (settings::g_misc.m_autobuy.primary_weapon)
                {
                case 1: cmd += xs("buy ak47; buy m4a1; "); break;
                case 2: cmd += xs("buy sg556; buy aug; "); break;
                case 3: cmd += xs("buy ssg08; "); break;
                case 4: cmd += xs("buy awp; "); break;
                case 5: cmd += xs("buy g3sg1; buy scar20; "); break;
                }

                if (settings::g_misc.m_autobuy.armor)
                {
                        cmd += xs("buy vesthelm; buy vest; ");
                }

                if (settings::g_misc.m_autobuy.taser)
                {
                        cmd += xs("buy taser; ");
                }

                if (settings::g_misc.m_autobuy.defuser)
                {
                        cmd += xs("buy defuser; ");
                }

                switch (settings::g_misc.m_autobuy.secondary_weapon)
                {
                case 1: cmd += xs("buy elite; "); break;
                case 2: cmd += xs("buy fiveseven; buy tec9; "); break;
                case 3: cmd += xs("buy deagle; "); break;
                case 4: cmd += xs("buy revolver; "); break;
                }

                for (auto i = 0; i < 5; ++i)
                {
                        if (!settings::g_misc.m_autobuy.grenades[i])
                        {
                                continue;
                        }

                        switch (i)
                        {
                        case 0: cmd += xs("buy molotov; buy incgrenade; "); break;
                        case 1: cmd += xs("buy hegrenade; "); break;
                        case 2: cmd += xs("buy smokegrenade; "); break;
                        case 3: cmd += xs("buy flashbang; "); break;
                        case 4: cmd += xs("buy decoy; "); break;
                        }
                }

                if (!cmd.empty())
                {
                        const auto engine = addresses::globals::source2engine_to_client;
                        const auto command_fn = PATTERN(patterns::engine_client_cmd);
                        if ( engine && engine >= 0x10000 && command_fn )
                                memory::safe_call<void>(command_fn, engine, 0, cmd.c_str(), 0x7ffef001);
                }
        }

        void other::do_player_alpha_changing()
        {
                const auto local = systems::g_local.get();

                if (!local.pawn || !local.is_alive)
                {
                        if (this->m_is_alpha_changed && local.pawn)
                        {
                                memory::call<void>(PATTERN(patterns::game_event_get_string), local.pawn, 255);
                        }

                        this->m_is_alpha_changed = false;
                        return;
                }

                if (!settings::g_esp.m_local_alpha.enabled.value)
                {
                        if (this->m_is_alpha_changed)
                        {
                                this->m_is_alpha_changed = false;
                                memory::call<void>(PATTERN(patterns::game_event_get_string), local.pawn, 255);
                        }

                        return;
                }

                const auto is_scoped = memory::read<bool>(local.pawn + SCHEMA("C_CSPlayerPawn", "m_bIsScoped"_hash));
                const auto should_apply = !settings::g_esp.m_local_alpha.only_scoped.value || is_scoped;

                if (should_apply)
                {
                        this->m_is_alpha_changed = true;
                        const auto alpha = static_cast<std::uint8_t>(settings::g_esp.m_local_alpha.opacity.value * 255.0f);
                        memory::call<void>(PATTERN(patterns::game_event_get_string), local.pawn, alpha);
                }
                else
                {
                        if (this->m_is_alpha_changed)
                        {
                                this->m_is_alpha_changed = false;
                                memory::call<void>(PATTERN(patterns::game_event_get_string), local.pawn, 255);
                        }
                }
        }

        void other::do_name_changing()
        {
                const auto local = systems::g_local.get();
                const auto& cfg = settings::g_misc.m_name_changer;
                const auto enabled = cfg.clantag.value || cfg.override_name.value || cfg.anim_nickname.value;

                if (!enabled)
                {
                        bool need_restore = this->m_name_changer_active;
                        if (!need_restore && local.controller)
                        {
                                const auto cur_name = controller_name(local.controller);
                                if (cur_name == "x")
                                {
                                        need_restore = true;
                                }
                        }

                        if (need_restore && local.controller)
                        {
                                auto steam_name = get_steam_nickname(this->m_name_changer_active ? 0 : local.controller);
                                if (steam_name.empty() || steam_name == "x")
                                {
                                        if (!this->m_original_name.empty() && this->m_original_name != "x")
                                        {
                                                steam_name = this->m_original_name;
                                        }
                                }

                                if (!steam_name.empty() && steam_name != "x" && this->m_last_sent_name != steam_name)
                                {
                                        submit_name_change(steam_name);
                                        this->m_last_sent_name = steam_name;
                                }
                        }

                        this->m_name_changer_active = false;
                        this->m_avatar_overridden = false;
                        this->m_name_changer_controller = 0;
                        this->m_original_name.clear();
                        this->m_original_steam_id = 0;
                        this->m_override_name_was_active = false;
                        return;
                }

                if (!local.controller)
                {
                        return;
                }

                if (this->m_name_changer_controller != local.controller)
                {
                        this->m_avatar_overridden = false;
                }

                // Once animation starts, the controller contains our own output.
                // Only Steam may refresh the source name while the feature is active.
                const auto steam_name = get_steam_nickname(this->m_name_changer_active ? 0 : local.controller);
                if (!steam_name.empty() && steam_name != "x")
                {
                        this->m_original_name = steam_name;
                }

                if (!this->m_name_changer_active)
                {
                        if (this->m_original_name.empty() || this->m_original_name == "x")
                        {
                                this->m_original_name = !steam_name.empty() ? steam_name : "Player";
                        }

                        this->m_name_changer_active = true;
                        this->m_name_changer_controller = local.controller;
                        this->m_last_sent_name.clear();
                }
                else if (this->m_name_changer_controller != local.controller)
                {
                        // Keep the captured real name across map loads, where the controller may be recreated.
                        this->m_name_changer_controller = local.controller;
                        this->m_last_sent_name.clear();
                }

                const bool random_nickname_active = cfg.override_name.value && cfg.random_nickname.value;

                // Randomly steal a name from the match player list, rotating at
                // a fast rate (one name per ~0.15s) while the checkbox is active.
                static std::string s_current_random_name;
                static auto s_next_random_time = std::chrono::steady_clock::now();
                if (random_nickname_active)
                {
                        const auto now = std::chrono::steady_clock::now();
                        if (s_current_random_name.empty() || now >= s_next_random_time)
                        {
                                auto candidate = random_match_player_name(s_current_random_name);
                                if (candidate.empty())
                                {
                                        candidate = make_random_nickname();
                                }

                                if (!candidate.empty())
                                {
                                        s_current_random_name = std::move(candidate);
                                        s_next_random_time = now + std::chrono::milliseconds(75);
                                }
                        }
                }
                else
                {
                        s_current_random_name.clear();
                }

                const bool override_name_active = cfg.override_name.value && (random_nickname_active || !cfg.name.value.empty());
                if (this->m_override_name_was_active && !override_name_active)
                {
                        // Override was toggled off while clantag is still active - force immediate update
                        this->m_last_sent_name.clear();
                }
                this->m_override_name_was_active = override_name_active;

                const auto& configured_name = random_nickname_active ? s_current_random_name : cfg.name.value;
                const auto& base_name = override_name_active
                        ? configured_name
                        : (!this->m_original_name.empty() && this->m_original_name != "x" ? this->m_original_name : (!steam_name.empty() && steam_name != "x" ? steam_name : "Player"));

                std::string animated_name = base_name;
                if (cfg.anim_nickname.value && !base_name.empty())
                {
                        const auto now = std::chrono::steady_clock::now();
                        static auto last_anim_time = now;
                        static std::uint64_t anim_step = 0;
                        static std::string last_source;
                        static int last_type = -1;
                        if (last_source != base_name || last_type != cfg.anim_type.value || this->m_last_sent_name.empty())
                        {
                                last_source = base_name;
                                last_type = cfg.anim_type.value;
                                anim_step = 0;
                                last_anim_time = now;
                        }

                        const auto speed = std::clamp(cfg.anim_speed.value, 0.05f, 2.0f);
                        const auto elapsed = std::chrono::duration<float>(now - last_anim_time).count();
                        if (elapsed >= speed)
                        {
                                last_anim_time = now;
                                anim_step++;
                        }

                        animated_name = nickname_animation::frame(base_name, cfg.anim_type.value, anim_step);
                }

                std::string display_name = animated_name;
                if (cfg.clantag.value)
                {
                        constexpr std::string_view tag{ "mintaly" };
                        constexpr auto ticks_per_step{ 32 }; // 0.25 seconds at CS2's 64-tick interval.
                        constexpr auto phase_count{ static_cast<int>(tag.size() * 2) };

                        const auto global_vars = memory::safe_read<std::uintptr_t>(addresses::globals::global_vars).value_or(0);
                        const auto current_tick = global_vars
                                ? memory::safe_read<int>(global_vars + 0x44).value_or(0)
                                : 0;
                        auto phase = current_tick / ticks_per_step % phase_count;
                        if (phase < 0)
                        {
                                phase += phase_count;
                        }

                        const auto reveal_index = phase <= static_cast<int>(tag.size())
                                ? phase
                                : phase_count - phase;
                        const auto visible_tag = tag.substr(0, static_cast<std::size_t>(reveal_index));
                        if (!visible_tag.empty())
                        {
                                display_name.reserve(base_name.size() + visible_tag.size() + 3);
                                display_name = "[";
                                display_name += visible_tag;
                                display_name += "] ";
                                display_name += animated_name;
                        }
                }

                display_name = utf8::bounded(display_name, 127, 127);
                if (display_name == this->m_last_sent_name)
                {
                        return;
                }

                submit_name_change(display_name);
                this->m_last_sent_name = std::move(display_name);
        }

        void other::do_kill_feed_preservation()
        {
                const auto local = systems::g_local.get();

                if (!local.pawn || !local.is_alive) {
                        return;
                }

                const auto hud_element = memory::call<std::uintptr_t>(PATTERN(patterns::find_hud_element), xs("CCSGO_HudDeathNotice"));
                if (!hud_element)
                {
                        return;
                }

                memory::write<float>(hud_element + 0x58, settings::g_misc.preserve_killfeed ? 1000.0f : 1.5f);

                float spawntime = memory::read<float>(local.pawn + SCHEMA("C_CSPlayerPawnBase", "m_flLastSpawnTimeIndex"_hash));
                if (m_last_spawntime != spawntime)
                {
                        const auto clear_death_notices = PATTERN(patterns::hud_death_notice_clear);
                        if (clear_death_notices)
                        {
                                memory::call<void>(clear_death_notices, hud_element - 0x20);
                        }

                        m_last_spawntime = spawntime;
                }
        }

        void other::vote_kick_self()
        {
                const auto local = systems::g_local.get();
                if ( !local.controller )
                {
                        return;
                }

                int local_slot = -1;
                for ( int i = 1; i <= 64; ++i )
                {
                        if ( systems::g_entities.get_by_index( i ) == local.controller )
                        {
                                local_slot = i - 1;
                                break;
                        }
                }

                if ( local_slot < 0 )
                {
                        return;
                }

                const auto cmd = std::format( "callvote kick {}", local_slot );
                const auto engine = addresses::globals::source2engine_to_client;
                const auto command_fn = PATTERN( patterns::engine_client_cmd );
                if ( engine && engine >= 0x10000 && command_fn )
                        memory::safe_call<void>( command_fn, engine, 0, cmd.c_str( ), 0x7ffef001 );
        }

        void other::do_chat_spam()
        {
                const auto& cfg = settings::g_misc.m_chat_spam;

                if (!cfg.enabled.value || cfg.message.value.empty())
                {
                        return;
                }

                const bool send_all  = cfg.targets.values[0];
                const bool send_team = cfg.targets.values[1];

                if (!send_all && !send_team)
                {
                        return;
                }

                using clock = std::chrono::steady_clock;
                static auto last_time = clock::now();

                const auto now = clock::now();
                const auto elapsed = std::chrono::duration<float>(now - last_time).count();

                if (elapsed < cfg.delay.value)
                {
                        return;
                }

                last_time = now;

                std::string text = cfg.message.value;
                std::replace(text.begin(), text.end(), '\n', ' ');
                std::replace(text.begin(), text.end(), '\r', ' ');
                std::replace(text.begin(), text.end(), '"', '\'');
                std::replace(text.begin(), text.end(), ';', ' ');

                if (send_all)
                {
                        const auto cmd = std::format("say \"{}\"", text);
                        const auto engine = addresses::globals::source2engine_to_client;
                        const auto command_fn = PATTERN(patterns::engine_client_cmd);
                        if ( engine && engine >= 0x10000 && command_fn )
                                memory::safe_call<void>(command_fn, engine, 0, cmd.c_str(), 0x7ffef001);
                }

                if (send_team)
                {
                        const auto cmd = std::format("say_team \"{}\"", text);
                        const auto engine = addresses::globals::source2engine_to_client;
                        const auto command_fn = PATTERN(patterns::engine_client_cmd);
                        if ( engine && engine >= 0x10000 && command_fn )
                                memory::safe_call<void>(command_fn, engine, 0, cmd.c_str(), 0x7ffef001);
                }
        }

} // namespace features::misc
