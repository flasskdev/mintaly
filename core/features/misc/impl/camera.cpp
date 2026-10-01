#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <utilities/logging/logging.hpp>
#include <core/systems/systems.hpp>
#include <core/settings.hpp>
#include <core/rendering/rendering.hpp>

#include "../misc.hpp"
#include <protection/game_addresses.hpp>

namespace features::misc {
	namespace {
		constexpr std::ptrdiff_t k_fov_offset{ 0x498 };
		constexpr std::ptrdiff_t k_aspect_ratio_offset{ 0x4d4 };
		constexpr std::ptrdiff_t k_view_flags_offset{ 0x551 };
		constexpr std::uint8_t k_explicit_aspect_ratio_flag{ 1u << 1 };

		// Smooth aspect ratio transitions, mirroring the viewmodel adjust interpolation
		// (exponential decay factor + per-frame lerp of the applied value).
		struct aspect_ratio_anim_state
		{
			bool initialized{ false };
			bool fading_out{ false };
			float current{ 1.333f };
			float native{ 1.777f };
			float fov_scale{ 1.0f };
			std::chrono::steady_clock::time_point last_time{};
		};

		inline aspect_ratio_anim_state s_aspect_anim{};

		// Smooth thirdperson transitions: the camera distance eases between 0
		// (first person) and the configured value instead of snapping.
		struct thirdperson_anim_state
		{
			bool was_enabled{ false };
			float current_distance{ 0.0f };
			std::chrono::steady_clock::time_point last_time{};
		};

		inline thirdperson_anim_state s_tp_anim{};

		// Smooth FOV transitions. Kept as file state so do_fov_change can stay const.
		struct fov_anim_state
		{
			bool initialized{ false };
			bool fading_out{ false };
			float current{ 90.0f };
			float native{ 90.0f };
			std::chrono::steady_clock::time_point last_time{};
		};

		inline fov_anim_state s_fov_anim{};

		// Tracks the moment the scope is released so the FOV can be ramped back out
		// from the scoped value instead of snapping to hipfire. Owns its own value so
		// the normal lerp cannot fight the ramp for control of the FOV.
		struct unzoom_hold_state
		{
			bool active{ false };
			bool scoped_now{ false };
			float value{ 90.0f };
			float goal{ 90.0f };
			std::chrono::steady_clock::time_point started{};
			std::chrono::steady_clock::time_point last_time{};
		};

		inline unzoom_hold_state s_unzoom_hold{};

		// Below this the FOV is visually indistinguishable from the engine value,
		// so the handover back to it is not noticeable.
		constexpr float k_fov_settle{ 0.05f };

		// Zoom level of the currently held weapon: 1 is the first magnification and
		// 2+ the secondary. Mirrors the legitbot's detection so both agree on what
		// counts as a second scope.
		[[nodiscard]] int read_zoom_level( std::uintptr_t target_pawn )
		{
			if ( !target_pawn )
			{
				return 1;
			}

			const auto weapon_services = memory::safe_read<std::uintptr_t>(
				target_pawn + SCHEMA( "C_BasePlayerPawn", "m_pWeaponServices"_hash ) ).value_or( 0 );
			if ( !weapon_services )
			{
				return 1;
			}

			const auto weapon_handle = memory::safe_read<std::uint32_t>(
				weapon_services + SCHEMA( "CPlayer_WeaponServices", "m_hActiveWeapon"_hash ) ).value_or( 0u );
			if ( !weapon_handle || weapon_handle == 0xffffffffu )
			{
				return 1;
			}

			const auto weapon = systems::g_entities.lookup( weapon_handle );
			if ( !weapon )
			{
				return 1;
			}

			int level = 1;

			const auto zoom_offset = SCHEMA( "C_CSWeaponBaseGun", "m_zoomLevel"_hash );
			if ( zoom_offset )
			{
				const auto value = memory::safe_read<int>( weapon + zoom_offset ).value_or( 1 );
				if ( value >= 2 )
				{
					level = 2;
				}
			}

			// Fallback: the camera services FOV is the ground truth for how far in the
			// player is zoomed. A value this low means a second scope even when the
			// weapon schema is unavailable.
			if ( level < 2 )
			{
				const auto cam_services = memory::safe_read<std::uintptr_t>(
					target_pawn + SCHEMA( "C_BasePlayerPawn", "m_pCameraServices"_hash ) ).value_or( 0 );
				if ( cam_services )
				{
					const auto fov_offset = SCHEMA( "CPlayer_CameraServices", "m_iFOV"_hash );
					if ( fov_offset )
					{
						const auto cur_fov = memory::safe_read<std::uint32_t>( cam_services + fov_offset ).value_or( 0u );
						if ( cur_fov > 0 && cur_fov <= 25 )
						{
							level = 2;
						}
					}
				}
			}

			return level;
		}

		// Below this the camera is close enough to the eye that the handover to the
		// engine's own first person view is not visible.
		constexpr float k_tp_settle_distance{ 0.5f };

		[[nodiscard]] bool is_valid_aspect( float value )
		{
			return std::isfinite( value ) && value > 0.1f && value < 10.0f;
		}

		// Aspect the engine projects with while our override is inactive. Used as
		// the transition origin so the first animated frame matches what is on screen.
		[[nodiscard]] float native_screen_aspect( )
		{
			const auto [ width, height ] = xdraw::viewport_size( );
			if ( width > 0 && height > 0 )
			{
				return static_cast< float >( width ) / static_cast< float >( height );
			}
			return 1.777f;
		}


		math::vector3 s_spec_freecam_angles{};
		bool s_was_spec_freecam{ false };

		math::vector3 s_spec_thirdperson_angles{};
		bool s_was_spec_thirdperson{ false };
		bool s_had_spec_mouse_event{ false };
		std::uintptr_t s_last_spec_pawn{ 0 };

		using SDL_GetRelativeMouseState_t = std::uint32_t( * )( float* x, float* y );
		using SDL_GetGlobalMouseState_t   = std::uint32_t( * )( float* x, float* y );

		inline SDL_GetRelativeMouseState_t s_get_relative_mouse_state = nullptr;
		inline SDL_GetGlobalMouseState_t   s_get_global_mouse_state   = nullptr;
		inline bool s_sdl_mouse_resolved = false;
		inline float s_last_global_x = 0.0f;
		inline float s_last_global_y = 0.0f;
		inline bool s_global_mouse_valid = false;
		inline POINT s_last_win_cursor{};
		inline bool s_win_cursor_valid = false;

		void query_mouse_delta( float& out_dx, float& out_dy )
		{
			out_dx = 0.0f;
			out_dy = 0.0f;

			if ( !s_sdl_mouse_resolved )
			{
				const auto sdl = GetModuleHandleA( "SDL3.dll" );
				if ( sdl )
				{
					s_get_relative_mouse_state = reinterpret_cast< SDL_GetRelativeMouseState_t >(
						GetProcAddress( sdl, "SDL_GetRelativeMouseState" )
					);
					s_get_global_mouse_state = reinterpret_cast< SDL_GetGlobalMouseState_t >(
						GetProcAddress( sdl, "SDL_GetGlobalMouseState" )
					);
				}
				s_sdl_mouse_resolved = true;
			}

			// 1. Primary: SDL_GetRelativeMouseState (native relative deltas in SDL3)
			if ( s_get_relative_mouse_state )
			{
				float r_dx = 0.0f, r_dy = 0.0f;
				__try
				{
					s_get_relative_mouse_state( &r_dx, &r_dy );
				}
				__except ( EXCEPTION_EXECUTE_HANDLER )
				{
					s_get_relative_mouse_state = nullptr;
				}

				if ( std::isfinite( r_dx ) && std::isfinite( r_dy ) )
				{
					if ( std::fabsf( r_dx ) > 0.0001f || std::fabsf( r_dy ) > 0.0001f )
					{
						out_dx = std::clamp( r_dx, -200.0f, 200.0f );
						out_dy = std::clamp( r_dy, -200.0f, 200.0f );
						return;
					}
				}
			}

			// 2. Secondary: SDL_GetGlobalMouseState
			if ( s_get_global_mouse_state )
			{
				float gx = 0.0f, gy = 0.0f;
				__try
				{
					s_get_global_mouse_state( &gx, &gy );
				}
				__except ( EXCEPTION_EXECUTE_HANDLER )
				{
					s_get_global_mouse_state = nullptr;
				}

				if ( std::isfinite( gx ) && std::isfinite( gy ) )
				{
					if ( !s_global_mouse_valid )
					{
						s_last_global_x = gx;
						s_last_global_y = gy;
						s_global_mouse_valid = true;
					}
					else
					{
						const float g_dx = gx - s_last_global_x;
						const float g_dy = gy - s_last_global_y;
						s_last_global_x = gx;
						s_last_global_y = gy;
						if ( std::fabsf( g_dx ) > 0.0001f || std::fabsf( g_dy ) > 0.0001f )
						{
							out_dx = std::clamp( g_dx, -200.0f, 200.0f );
							out_dy = std::clamp( g_dy, -200.0f, 200.0f );
							return;
						}
					}
				}
			}

			// 3. Fallback: Windows cursor delta (without SetCursorPos)
			POINT cur{};
			if ( GetCursorPos( &cur ) )
			{
				if ( !s_win_cursor_valid )
				{
					s_last_win_cursor = cur;
					s_win_cursor_valid = true;
				}
				else
				{
					const float w_dx = static_cast< float >( cur.x - s_last_win_cursor.x );
					const float w_dy = static_cast< float >( cur.y - s_last_win_cursor.y );
					s_last_win_cursor = cur;
					if ( std::fabsf( w_dx ) > 0.0001f || std::fabsf( w_dy ) > 0.0001f )
					{
						out_dx = std::clamp( w_dx, -200.0f, 200.0f );
						out_dy = std::clamp( w_dy, -200.0f, 200.0f );
						return;
					}
				}
			}
		}

		[[nodiscard]] float scale_horizontal_fov( float fov, float aspect_ratio )
		{
			constexpr auto degrees_to_half_radians{ std::numbers::pi_v<float> / 360.0f };
			constexpr auto half_radians_to_degrees{ 360.0f / std::numbers::pi_v<float> };
			constexpr auto four_by_three_inverse{ 0.75f };

			return std::atan( std::tan( fov * degrees_to_half_radians ) * aspect_ratio * four_by_three_inverse ) * half_radians_to_degrees;
		}
	}

	void camera::on_override_view( std::uintptr_t view_setup )
	{
		if ( !view_setup )
		{
			return;
		}

		const auto local = systems::g_local.get( );
		if ( !systems::g_local.is_in_cinematic( ) )
		{
			const auto target_pawn = local.view_pawn( );
			const bool is_spec = !local.is_alive && local.observer_pawn != 0;
			const bool allow_thirdperson = local.is_alive ? settings::g_misc.m_camera.thirdperson.value : ( settings::g_misc.m_camera.spectator_thirdperson.value && is_spec );

			// Keep driving the camera while an animated thirdperson eases back into
			// first person, otherwise the toggle-off handover to the engine would snap.
			const bool animated_tp = settings::g_misc.m_camera.thirdperson_animated.value;
			const bool fading_thirdperson = animated_tp &&
				!allow_thirdperson &&
				s_tp_anim.current_distance > k_tp_settle_distance;

			// Animation was switched off mid fade: drop the residual distance so we
			// stop driving the camera, otherwise it would stay stuck part-way out.
			if ( !animated_tp && !allow_thirdperson )
			{
				s_tp_anim.current_distance = 0.0f;
				s_tp_anim.was_enabled = false;
			}

			if ( this->do_freecam( view_setup ) )
			{
				// Freecam handled camera position and angles.
			}
			else if ( target_pawn && ( allow_thirdperson || fading_thirdperson ) )
			{
				this->do_thirdperson( view_setup, target_pawn );
			}
			else if ( !target_pawn && fading_thirdperson )
			{
				// Pawn went away mid fade; nothing left to interpolate against.
				s_tp_anim.current_distance = 0.0f;
				s_tp_anim.was_enabled = false;
			}

			if ( target_pawn )
			{
				this->do_fov_change( view_setup, target_pawn );
			}
			else if ( s_fov_anim.initialized && !settings::g_misc.m_camera.change_fov.value )
			{
				// No pawn to read a native FOV from, so the fade has nothing to
				// converge on. Drop it rather than leaving it stuck mid-transition.
				s_fov_anim.initialized = false;
			}
		}

		// Aspect conversion must run after the base FOV has been selected.
		this->do_aspect_ratio_change( view_setup );
	}

	void camera::update_fov_sensitivity( std::uintptr_t player_pawn ) const
	{
		if ( !settings::g_misc.m_camera.change_fov.value )
		{
			return;
		}

		const auto local = systems::g_local.get( );
		if ( player_pawn != local.pawn )
		{
			return;
		}

		const auto& cfg = settings::g_misc.m_camera;
		const auto is_scoped = memory::read<bool>( player_pawn + SCHEMA( "C_CSPlayerPawn", "m_bIsScoped"_hash ) );

		// Track the FOV actually applied this frame rather than the raw config value,
		// otherwise zoom sensitivity snaps to the final value while the view is still
		// easing toward it.
		const auto target_fov = s_fov_anim.initialized ? s_fov_anim.current : cfg.fov.value;

		if ( is_scoped == this->m_cached_scoped && target_fov == this->m_cached_target_fov && this->m_cached_fov_sensitivity >= 0.0f )
		{
			const auto current_adjust = memory::read<float>( player_pawn + SCHEMA( "C_BasePlayerPawn", "m_flFOVSensitivityAdjust"_hash ) );
			if ( std::fabsf( current_adjust - this->m_cached_fov_sensitivity ) < 0.0001f )
			{
				return;
			}
		}

		this->m_cached_scoped = is_scoped;
		this->m_cached_target_fov = target_fov;

		const auto cvar = CONVAR( "zoom_sensitivity_ratio" );
		const auto ratio = cvar ? cvar->get<float>( ) : 1.0f;
		const auto desired = ratio * ( target_fov / 90.0f );

		this->m_cached_fov_sensitivity = desired;
		memory::safe_write<float>( player_pawn + SCHEMA( "C_BasePlayerPawn", "m_flFOVSensitivityAdjust"_hash ), desired );
	}

	void camera::do_thirdperson( std::uintptr_t view_setup, std::uintptr_t target_pawn )
	{
		if ( !view_setup || !target_pawn )
		{
			return;
		}

		const auto& cfg = settings::g_misc.m_camera;
		const auto local = systems::g_local.get( );
		const bool tp_enabled = local.is_alive ? cfg.thirdperson.value : ( cfg.spectator_thirdperson.value && local.observer_pawn != 0 );

		const bool animated = cfg.thirdperson_animated.value;
		const float target_distance = std::clamp( cfg.thirdperson_distance.value, 10.0f, 500.0f );

		// With animation on, keep driving the camera while the distance eases back
		// toward zero so the pull into first person is gradual. Without it, the
		// engine snaps back the moment the toggle goes off.
		const bool fading_out = animated && !tp_enabled && s_tp_anim.current_distance > k_tp_settle_distance;

		if ( !tp_enabled && !fading_out )
		{
			if ( animated )
			{
				s_tp_anim.current_distance = 0.0f;
				s_tp_anim.was_enabled = false;
				s_tp_anim.last_time = std::chrono::steady_clock::now( );
			}
			s_was_spec_thirdperson = false;
			return;
		}

		float distance = target_distance;

		if ( animated )
		{
			const auto now = std::chrono::steady_clock::now( );

			// Fade out toward the eye, fade in toward the configured distance.
			const float goal = tp_enabled ? target_distance : 0.0f;

			// Detect the toggle edge. Resetting the clock here matters: while
			// disabled this function stops being called, so last_time would be
			// stale and the first animated frame would jump most of the way.
			if ( s_tp_anim.was_enabled != tp_enabled )
			{
				s_tp_anim.was_enabled = tp_enabled;
				s_tp_anim.last_time = now;
			}

			float dt = std::chrono::duration<float>( now - s_tp_anim.last_time ).count( );
			s_tp_anim.last_time = now;
			dt = std::clamp( dt, 0.0f, 0.1f );

			if ( dt > 0.0f )
			{
				constexpr float k_animation_speed = 12.0f;
				const float factor = 1.0f - std::exp( -k_animation_speed * dt );
				s_tp_anim.current_distance = std::lerp( s_tp_anim.current_distance, goal, factor );
			}

			if ( std::abs( s_tp_anim.current_distance - goal ) < 0.1f )
			{
				s_tp_anim.current_distance = goal;
			}

			distance = s_tp_anim.current_distance;
		}
		else
		{
			s_tp_anim.current_distance = target_distance;
			s_tp_anim.was_enabled = tp_enabled;
			s_tp_anim.last_time = std::chrono::steady_clock::now( );
		}

		const auto game_scene_node = memory::safe_read<std::uintptr_t>( target_pawn + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) ).value_or( 0 );
		math::vector3 eye_position = memory::safe_read<math::vector3>( view_setup + 0x4a0 ).value_or( math::vector3{} );
		if ( game_scene_node )
		{
			const auto origin = memory::safe_read<math::vector3>( game_scene_node + SCHEMA( "CGameSceneNode", "m_vecAbsOrigin"_hash ) ).value_or( math::vector3{} );
			const auto view_offset = memory::safe_read<math::vector3>( target_pawn + SCHEMA( "C_BaseModelEntity", "m_vecViewOffset"_hash ) ).value_or( math::vector3{} );
			if ( origin.length_sqr( ) > 0.0f && std::isfinite( origin.x ) && std::isfinite( origin.y ) && std::isfinite( origin.z ) &&
			     std::isfinite( view_offset.x ) && std::isfinite( view_offset.y ) && std::isfinite( view_offset.z ) )
			{
				eye_position = origin + view_offset;
			}
		}

		if ( !std::isfinite( eye_position.x ) || !std::isfinite( eye_position.y ) || !std::isfinite( eye_position.z ) )
		{
			return;
		}

		math::vector3 view_angles{};
		if ( local.is_alive )
		{
			s_was_spec_thirdperson = false;
			view_angles = systems::g_input.get_view_angles( );
		}
		else
		{
			// Free camera orbital rotation around the spectated player
			if ( !s_was_spec_thirdperson || s_last_spec_pawn != target_pawn )
			{
				s_spec_thirdperson_angles = memory::safe_read<math::vector3>( target_pawn + SCHEMA( "C_CSPlayerPawn", "m_angEyeAngles"_hash ) ).value_or( math::vector3{} );
				if ( !std::isfinite( s_spec_thirdperson_angles.x ) || !std::isfinite( s_spec_thirdperson_angles.y ) || !std::isfinite( s_spec_thirdperson_angles.z ) || s_spec_thirdperson_angles.length_sqr( ) < 0.001f )
				{
					s_spec_thirdperson_angles = memory::safe_read<math::vector3>( view_setup + 0x4b8 ).value_or( math::vector3{} );
				}
				s_spec_thirdperson_angles.x = std::clamp( s_spec_thirdperson_angles.x, -89.0f, 89.0f );
				s_spec_thirdperson_angles.y = math::helpers::normalize_yaw( s_spec_thirdperson_angles.y );
				s_spec_thirdperson_angles.z = 0.0f;

				s_was_spec_thirdperson = true;
				s_last_spec_pawn = target_pawn;
			}

			// Fallback mouse look if WM_INPUT was not handled
			if ( !s_had_spec_mouse_event && !rendering::g_menu.is_open( ) )
			{
				float dx = 0.0f, dy = 0.0f;
				query_mouse_delta( dx, dy );
				if ( std::fabsf( dx ) > 0.0001f || std::fabsf( dy ) > 0.0001f )
				{
					float sens = 1.0f;
					if ( const auto cvar = CONVAR( "sensitivity" ) ) sens = cvar->get< float >( );
					sens = std::clamp( sens, 0.001f, 100.0f );

					float m_pitch = 0.022f;
					if ( const auto cvar = CONVAR( "m_pitch" ) ) m_pitch = cvar->get< float >( );
					float m_yaw = 0.022f;
					if ( const auto cvar = CONVAR( "m_yaw" ) ) m_yaw = cvar->get< float >( );

					s_spec_thirdperson_angles.x = std::clamp( s_spec_thirdperson_angles.x + dy * m_pitch * sens, -89.0f, 89.0f );
					s_spec_thirdperson_angles.y = math::helpers::normalize_yaw( s_spec_thirdperson_angles.y - dx * m_yaw * sens );
					s_spec_thirdperson_angles.z = 0.0f;
				}
			}
			s_had_spec_mouse_event = false;

			view_angles = s_spec_thirdperson_angles;
		}

		if ( !std::isfinite( view_angles.x ) || !std::isfinite( view_angles.y ) || !std::isfinite( view_angles.z ) )
		{
			view_angles = {};
		}

		view_angles.x = std::clamp( view_angles.x, -89.0f, 89.0f );
		view_angles.y = math::helpers::normalize_yaw( view_angles.y );
		view_angles.z = 0.0f;

		math::vector3 forward{};
		math::helpers::angle_vectors_left( view_angles, &forward );

		if ( !std::isfinite( forward.x ) || !std::isfinite( forward.y ) || !std::isfinite( forward.z ) )
		{
			forward = { 1.0f, 0.0f, 0.0f };
		}

		const float hull_size = std::clamp( cfg.thirdperson_hull_size.value, 0.0f, 50.0f );

		auto camera_position = eye_position - forward * distance;

		if ( std::isfinite( camera_position.x ) && std::isfinite( camera_position.y ) && std::isfinite( camera_position.z ) )
		{
			if ( hull_size > 0.001f )
			{
				if ( hull_size > 0.1f )
				{
					const auto hull_mins = math::vector3{ -hull_size, -hull_size, -hull_size };
					const auto hull_maxs = math::vector3{ hull_size, hull_size, hull_size };
					const auto result = systems::g_tracing.trace_hull( eye_position, camera_position, hull_mins, hull_maxs, target_pawn );

					if ( result.fraction < 1.0f )
					{
						const auto world = systems::g_entities.get_by_index( 0 );
						if ( result.hit_entity == world || result.hit_entity == 0 )
						{
							camera_position = eye_position + ( camera_position - eye_position ) * result.fraction;
						}
					}
				}
				else
				{
					const auto result = systems::g_tracing.trace( eye_position, camera_position, target_pawn );
					if ( result.fraction < 1.0f )
					{
						const auto world = systems::g_entities.get_by_index( 0 );
						if ( result.hit_entity == world || result.hit_entity == 0 )
						{
							camera_position = eye_position + ( camera_position - eye_position ) * result.fraction;
						}
					}
				}
			}
			// If hull_size <= 0.001f, wall tracing is disabled: camera passes through walls freely
		}

		if ( std::isfinite( camera_position.x ) && std::isfinite( camera_position.y ) && std::isfinite( camera_position.z ) )
		{
			memory::safe_write<math::vector3>( view_setup + 0x4a0, camera_position );
		}
		if ( !local.is_alive )
		{
			memory::safe_write<math::vector3>( view_setup + 0x4b8, view_angles );
		}
	}

	void camera::do_fov_change( std::uintptr_t view_setup, std::uintptr_t target_pawn ) const
	{
		if ( !view_setup || !target_pawn )
		{
			return;
		}

		const auto& cfg = settings::g_misc.m_camera;
		const auto now = std::chrono::steady_clock::now( );

		// What the engine picked for this frame, read before anything overwrites it.
		const auto engine_fov = memory::safe_read<float>( view_setup + k_fov_offset ).value_or( 90.0f );

		if ( !cfg.change_fov.value )
		{
			// Keep easing back to the engine value while the override is released.
			const bool fading = s_fov_anim.initialized && s_fov_anim.current - s_fov_anim.native > k_fov_settle;

			if ( !s_fov_anim.initialized )
			{
				s_fov_anim.last_time = now;
				return;
			}

			s_fov_anim.native = std::isfinite( engine_fov ) ? engine_fov : s_fov_anim.native;

			if ( !fading )
			{
				// Settled: stop touching the FOV so the engine owns it again.
				s_fov_anim.initialized = false;
				s_fov_anim.current = s_fov_anim.native;
				s_fov_anim.last_time = now;
				return;
			}

			float dt = std::chrono::duration<float>( now - s_fov_anim.last_time ).count( );
			s_fov_anim.last_time = now;
			dt = std::clamp( dt, 0.0f, 0.1f );

			if ( dt > 0.0f )
			{
				constexpr float k_animation_speed = 15.0f;
				const float factor = 1.0f - std::exp( -k_animation_speed * dt );
				s_fov_anim.current = std::lerp( s_fov_anim.current, s_fov_anim.native, factor );
			}

			if ( std::abs( s_fov_anim.current - s_fov_anim.native ) < k_fov_settle )
			{
				s_fov_anim.initialized = false;
				return;
			}

			memory::safe_write<float>( view_setup + k_fov_offset, s_fov_anim.current );
			return;
		}

		const auto is_scoped = memory::safe_read<bool>( target_pawn + SCHEMA( "C_CSPlayerPawn", "m_bIsScoped"_hash ) ).value_or( false );
		const auto zoom_level = is_scoped ? read_zoom_level( target_pawn ) : 1;

		// Pick the FOV for the current magnification stage. Level 2+ falls back to the
		// first-scope value so weapons without a second scope are unaffected.
		float target = cfg.fov.value;
		if ( is_scoped )
		{
			if ( zoom_level >= 2 && cfg.scoped_fov2_override.value )
			{
				target = cfg.scoped_fov2.value;
			}
			else if ( cfg.scoped_fov_override.value )
			{
				target = cfg.scoped_fov.value;
			}
		}

		const auto goal = std::clamp( target, 5.0f, 170.0f );

		if ( !cfg.animated_unzoom.value )
		{
			s_unzoom_hold = {};
		}
		else if ( is_scoped )
		{
			// Scoped: cancel any in-flight unzoom and remember we are scoped so the
			// release below can be detected.
			s_unzoom_hold.active = false;
			s_unzoom_hold.scoped_now = true;
		}
		else if ( s_unzoom_hold.scoped_now )
		{
			// Scope just released. Start the ramp from the FOV currently on screen
			// toward the hipfire goal.
			s_unzoom_hold.scoped_now = false;
			s_unzoom_hold.active = true;
			s_unzoom_hold.started = now;
			s_unzoom_hold.last_time = now;
			s_unzoom_hold.goal = goal;
			s_unzoom_hold.value = s_fov_anim.initialized
				? s_fov_anim.current
				: ( std::isfinite( engine_fov ) && engine_fov > 1.0f ? engine_fov : goal );
		}
		else if ( s_unzoom_hold.active )
		{
			// Track the goal each frame: if the player swaps weapons or the config
			// changes mid-ramp we must converge on the new value, not the old one.
			s_unzoom_hold.goal = goal;
		}

		float dt = std::chrono::duration<float>( now - s_fov_anim.last_time ).count( );
		s_fov_anim.last_time = now;
		dt = std::clamp( dt, 0.0f, 0.1f );

		if ( s_unzoom_hold.active )
		{
			// Ramp from the captured value toward the goal. Speed is configurable so a
			// slow pull-out can be dialled in independently of the normal lerp.
			const float speed = std::clamp( cfg.unzoom_speed.value, 2.0f, 30.0f );
			const float factor = 1.0f - std::exp( -speed * dt );

			if ( dt > 0.0f )
			{
				s_unzoom_hold.value = std::lerp( s_unzoom_hold.value, s_unzoom_hold.goal, factor );
			}

			if ( std::abs( s_unzoom_hold.value - s_unzoom_hold.goal ) < k_fov_settle )
			{
				// Converged: hand back to the normal path so future frames settle on
				// the goal exactly instead of asymptotically creeping toward it.
				s_unzoom_hold.value = s_unzoom_hold.goal;
				s_unzoom_hold.active = false;
				s_fov_anim.current = s_unzoom_hold.goal;
				s_fov_anim.initialized = true;
				s_fov_anim.native = s_unzoom_hold.goal;
			}
			else
			{
				s_fov_anim.current = s_unzoom_hold.value;
			}
		}
		else if ( !s_fov_anim.initialized )
		{
			// Ease out of whatever the engine is already showing instead of jumping.
			s_fov_anim.current = std::isfinite( engine_fov ) && engine_fov > 1.0f ? engine_fov : goal;
			s_fov_anim.native = s_fov_anim.current;
			s_fov_anim.initialized = true;
		}
		else if ( dt >= 0.1f )
		{
			// A long gap (alt-tab, respawn) would otherwise produce one huge step.
			s_fov_anim.current = goal;
		}
		else if ( dt > 0.0f )
		{
			constexpr float k_animation_speed = 15.0f;
			const float factor = 1.0f - std::exp( -k_animation_speed * dt );
			s_fov_anim.current = std::lerp( s_fov_anim.current, goal, factor );
		}

		if ( std::abs( s_fov_anim.current - goal ) < k_fov_settle )
		{
			s_fov_anim.current = goal;
			// Keep native in sync so a later toggle-off fades back to the right value.
			if ( !s_unzoom_hold.active )
			{
				s_fov_anim.native = goal;
			}
		}

		memory::safe_write<float>( view_setup + k_fov_offset, s_fov_anim.current );

		this->update_fov_sensitivity( target_pawn );
	}

	void camera::do_aspect_ratio_change( std::uintptr_t view_setup )
	{
		if ( !view_setup )
		{
			return;
		}

		const auto& cfg = settings::g_misc.m_camera;
		const auto now = std::chrono::steady_clock::now( );
		const auto flags = memory::safe_read<std::uint8_t>( view_setup + k_view_flags_offset ).value_or( 0 );
		const bool enabled = cfg.change_aspect_ratio.value;

		if ( enabled && s_aspect_anim.fading_out )
		{
			// Re-enabled mid fade-out: keep the current value and let it blend onward.
			s_aspect_anim.fading_out = false;
		}

		if ( !enabled && s_aspect_anim.initialized && !s_aspect_anim.fading_out )
		{
			// Just got disabled: smoothly blend back to the engine's native aspect ratio.
			s_aspect_anim.fading_out = true;
		}

		if ( !enabled && !s_aspect_anim.initialized )
		{
			// Nothing to blend: release the flag so the engine drives the aspect itself.
			memory::safe_write<std::uint8_t>( view_setup + k_view_flags_offset,
				flags & static_cast<std::uint8_t>( ~k_explicit_aspect_ratio_flag ) );
			return;
		}

		const auto target = enabled
			? std::clamp( cfg.aspect_ratio.value, 0.5f, 3.0f )
			: s_aspect_anim.native;
		const bool just_initialized = !s_aspect_anim.initialized;

		if ( just_initialized )
		{
			// Start from whatever the engine currently applies so enabling the
			// feature blends out of the native aspect instead of snapping to it.
			const auto engine_aspect = memory::safe_read<float>( view_setup + k_aspect_ratio_offset ).value_or( 0.0f );
			s_aspect_anim.native = is_valid_aspect( engine_aspect ) ? engine_aspect : native_screen_aspect( );
			s_aspect_anim.current = s_aspect_anim.native;
			s_aspect_anim.last_time = now;
			s_aspect_anim.initialized = true;
		}

		float dt = std::chrono::duration<float>( now - s_aspect_anim.last_time ).count( );
		s_aspect_anim.last_time = now;
		dt = std::clamp( dt, 0.0f, 0.1f );

		// Skip the lerp on the first frame so we hold the starting aspect instead
		// of jumping to the target before the transition has a chance to run.
		if ( !just_initialized && dt > 0.0f )
		{
			constexpr float k_animation_speed = 15.0f;
			const float factor = 1.0f - std::exp( -k_animation_speed * dt );
			s_aspect_anim.current = std::lerp( s_aspect_anim.current, target, factor );
		}

		// Snap once the difference is below what the projection matrix can resolve.
		if ( std::abs( s_aspect_anim.current - target ) < 0.0005f )
		{
			s_aspect_anim.current = target;
		}

		// Fade-out finished: hand control back to the engine so the final frame
		// matches its native projection exactly without jerking.
		if ( s_aspect_anim.fading_out && s_aspect_anim.current == target )
		{
			s_aspect_anim.initialized = false;
			s_aspect_anim.fading_out = false;
			s_aspect_anim.current = target;
			s_aspect_anim.native = native_screen_aspect( );
			s_aspect_anim.fov_scale = 1.0f;
			s_aspect_anim.last_time = now;

			if ( s_fov_anim.initialized )
			{
				memory::safe_write<float>( view_setup + k_fov_offset, s_fov_anim.current );
			}
			memory::safe_write<float>( view_setup + k_aspect_ratio_offset, target );
			memory::safe_write<std::uint8_t>( view_setup + k_view_flags_offset,
				flags & static_cast<std::uint8_t>( ~k_explicit_aspect_ratio_flag ) );
			return;
		}

		const auto base_fov = memory::safe_read<float>( view_setup + k_fov_offset ).value_or( 90.0f );
		const auto scaled_fov = scale_horizontal_fov( base_fov, s_aspect_anim.current );
		s_aspect_anim.fov_scale = ( base_fov > 0.0f ) ? ( scaled_fov / base_fov ) : 1.0f;

		// Explicit aspect: apply scaled horizontal FOV and aspect ratio
		memory::safe_write<float>( view_setup + k_fov_offset, scaled_fov );
		memory::safe_write<float>( view_setup + k_aspect_ratio_offset, s_aspect_anim.current );
		memory::safe_write<std::uint8_t>( view_setup + k_view_flags_offset, flags | k_explicit_aspect_ratio_flag );
	}

	float camera::aspect_fov_scale( ) const noexcept
	{
		return s_aspect_anim.fov_scale;
	}

	float camera::aspect_viewmodel_scale( ) const noexcept
	{
		if ( !s_aspect_anim.initialized || s_aspect_anim.native <= 0.0f )
			return 1.0f;
		return s_aspect_anim.current / s_aspect_anim.native;
	}


	bool camera::is_spec_thirdperson_active( ) const noexcept
	{
		const auto local = systems::g_local.get( );
		return !local.is_alive && local.observer_pawn != 0 && settings::g_misc.m_camera.spectator_thirdperson.value && !this->m_was_freecam_active;
	}

	void camera::on_spec_thirdperson_mouse_delta( float d_pitch, float d_yaw )
	{
		if ( !this->is_spec_thirdperson_active( ) || rendering::g_menu.is_open( ) )
		{
			return;
		}

		if ( ( std::fabsf( d_pitch ) > 0.0001f || std::fabsf( d_yaw ) > 0.0001f ) && std::isfinite( d_pitch ) && std::isfinite( d_yaw ) )
		{
			s_spec_thirdperson_angles.x = std::clamp( s_spec_thirdperson_angles.x + d_pitch, -89.0f, 89.0f );
			s_spec_thirdperson_angles.y = math::helpers::normalize_yaw( s_spec_thirdperson_angles.y + d_yaw );
			s_spec_thirdperson_angles.z = 0.0f;
			s_had_spec_mouse_event = true;
		}
	}

	void camera::on_mouse_delta( float d_pitch, float d_yaw )
	{
		if ( !this->m_was_freecam_active || rendering::g_menu.is_open( ) )
		{
			return;
		}

		if ( ( std::fabsf( d_pitch ) > 0.0001f || std::fabsf( d_yaw ) > 0.0001f ) && std::isfinite( d_pitch ) && std::isfinite( d_yaw ) )
		{
			this->m_freecam_angles.x = std::clamp( this->m_freecam_angles.x + d_pitch, -89.0f, 89.0f );
			this->m_freecam_angles.y = math::helpers::normalize_yaw( this->m_freecam_angles.y + d_yaw );
			this->m_freecam_angles.z = 0.0f;
			this->m_had_mouse_event = true;
		}
	}

	bool camera::do_freecam( std::uintptr_t view_setup )
	{
		if ( !view_setup )
		{
			return false;
		}

		const auto& cfg = settings::g_misc.m_camera;
		if ( !cfg.freecam.value )
		{
			if ( this->m_was_freecam_active )
			{
				if ( std::isfinite( this->m_saved_viewangles.x ) && std::isfinite( this->m_saved_viewangles.y ) && std::isfinite( this->m_saved_viewangles.z ) )
				{
					systems::g_input.set_view_angles( this->m_saved_viewangles );
				}

				this->m_was_freecam_active = false;
				this->m_had_mouse_event = false;
				this->m_cmd_buttons = 0;
				s_was_spec_freecam = false;
				s_global_mouse_valid = false;
				s_win_cursor_valid = false;
			}
			return false;
		}

		const auto now = std::chrono::steady_clock::now( );
		if ( !this->m_was_freecam_active )
		{
			this->m_freecam_pos = memory::safe_read<math::vector3>( view_setup + 0x4a0 ).value_or( math::vector3{} );
			if ( !std::isfinite( this->m_freecam_pos.x ) || !std::isfinite( this->m_freecam_pos.y ) || !std::isfinite( this->m_freecam_pos.z ) || this->m_freecam_pos.length_sqr( ) < 1.0f )
			{
				const auto local = systems::g_local.get( );
				const auto view_pawn = local.view_pawn( );
				if ( view_pawn )
				{
					const auto game_scene_node = memory::safe_read<std::uintptr_t>( view_pawn + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) ).value_or( 0 );
					if ( game_scene_node )
					{
						const auto origin = memory::safe_read<math::vector3>( game_scene_node + SCHEMA( "CGameSceneNode", "m_vecAbsOrigin"_hash ) ).value_or( math::vector3{} );
						const auto view_offset = memory::safe_read<math::vector3>( view_pawn + SCHEMA( "C_BaseModelEntity", "m_vecViewOffset"_hash ) ).value_or( math::vector3{} );
						if ( origin.length_sqr( ) > 0.0f && std::isfinite( origin.x ) && std::isfinite( origin.y ) && std::isfinite( origin.z ) &&
						     std::isfinite( view_offset.x ) && std::isfinite( view_offset.y ) && std::isfinite( view_offset.z ) )
						{
							this->m_freecam_pos = origin + view_offset;
						}
					}
				}
				if ( this->m_freecam_pos.length_sqr( ) < 1.0f )
				{
					this->m_freecam_pos = systems::g_view.origin( );
				}
			}

			this->m_saved_viewangles = systems::g_input.get_view_angles( );
			this->m_freecam_angles = memory::safe_read<math::vector3>( view_setup + 0x4b8 ).value_or( math::vector3{} );
			if ( !std::isfinite( this->m_freecam_angles.x ) || !std::isfinite( this->m_freecam_angles.y ) || !std::isfinite( this->m_freecam_angles.z ) || this->m_freecam_angles.length_sqr( ) < 0.001f )
			{
				this->m_freecam_angles = this->m_saved_viewangles;
			}
			this->m_freecam_angles.x = std::clamp( this->m_freecam_angles.x, -89.0f, 89.0f );
			this->m_freecam_angles.y = math::helpers::normalize_yaw( this->m_freecam_angles.y );
			this->m_freecam_angles.z = 0.0f;

			this->m_last_override_time = now;
			this->m_was_freecam_active = true;
			this->m_had_mouse_event = false;
			s_was_spec_freecam = false;
			s_global_mouse_valid = false;
			s_win_cursor_valid = false;
		}

		float dt = 0.016f;
		if ( this->m_last_override_time.time_since_epoch( ).count( ) > 0 )
		{
			const float raw_dt = std::chrono::duration<float>( now - this->m_last_override_time ).count( );
			if ( raw_dt >= 0.001f )
			{
				dt = std::clamp( raw_dt, 0.001f, 0.1f );
				this->m_last_override_time = now;
			}
			else
			{
				dt = 0.0f;
			}
		}
		else
		{
			this->m_last_override_time = now;
			dt = 0.016f;
		}

		// Fallback mouse look (when WM_INPUT wasn't handled or during spectator)
		if ( !this->m_had_mouse_event && !rendering::g_menu.is_open( ) )
		{
			float dx = 0.0f, dy = 0.0f;
			query_mouse_delta( dx, dy );
			if ( std::fabsf( dx ) > 0.0001f || std::fabsf( dy ) > 0.0001f )
			{
				float sens = 1.0f;
				if ( const auto cvar = CONVAR( "sensitivity" ) ) sens = cvar->get< float >( );
				sens = std::clamp( sens, 0.001f, 100.0f );

				float m_pitch = 0.022f;
				if ( const auto cvar = CONVAR( "m_pitch" ) ) m_pitch = cvar->get< float >( );
				float m_yaw = 0.022f;
				if ( const auto cvar = CONVAR( "m_yaw" ) ) m_yaw = cvar->get< float >( );

				this->m_freecam_angles.x = std::clamp( this->m_freecam_angles.x + dy * m_pitch * sens, -89.0f, 89.0f );
				this->m_freecam_angles.y = math::helpers::normalize_yaw( this->m_freecam_angles.y - dx * m_yaw * sens );
				this->m_freecam_angles.z = 0.0f;
			}
		}
		this->m_had_mouse_event = false;

		const bool can_move = !rendering::g_menu.is_open( );
		if ( can_move && dt > 0.0f )
		{
			math::vector3 forward{};
			math::vector3 right{};
			math::helpers::angle_vectors_left( this->m_freecam_angles, &forward, &right );

			math::vector3 move_dir{};

			const bool move_fwd = ( GetAsyncKeyState( 'W' ) & 0x8000 ) || ( GetAsyncKeyState( VK_UP ) & 0x8000 ) || ( this->m_cmd_buttons & cstypes::command_buttons::in_forward );
			const bool move_back = ( GetAsyncKeyState( 'S' ) & 0x8000 ) || ( GetAsyncKeyState( VK_DOWN ) & 0x8000 ) || ( this->m_cmd_buttons & cstypes::command_buttons::in_back );
			const bool move_left = ( GetAsyncKeyState( 'A' ) & 0x8000 ) || ( GetAsyncKeyState( VK_LEFT ) & 0x8000 ) || ( this->m_cmd_buttons & cstypes::command_buttons::in_moveleft );
			const bool move_right = ( GetAsyncKeyState( 'D' ) & 0x8000 ) || ( GetAsyncKeyState( VK_RIGHT ) & 0x8000 ) || ( this->m_cmd_buttons & cstypes::command_buttons::in_moveright );
			const bool move_up = ( GetAsyncKeyState( VK_SPACE ) & 0x8000 ) || ( this->m_cmd_buttons & cstypes::command_buttons::in_jump );
			const bool move_down = ( GetAsyncKeyState( VK_CONTROL ) & 0x8000 ) || ( GetAsyncKeyState( 'C' ) & 0x8000 ) || ( this->m_cmd_buttons & cstypes::command_buttons::in_duck );

			if ( move_fwd ) move_dir += forward;
			if ( move_back ) move_dir -= forward;
			if ( move_left ) move_dir -= right;
			if ( move_right ) move_dir += right;
			if ( move_up ) move_dir.z += 1.0f;
			if ( move_down ) move_dir.z -= 1.0f;

			float speed = std::clamp( cfg.freecam_speed.value, 100.0f, 10000.0f );
			if ( ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) || ( this->m_cmd_buttons & cstypes::command_buttons::in_sprint ) )
			{
				speed *= 2.5f;
			}
			else if ( ( GetAsyncKeyState( VK_MENU ) & 0x8000 ) )
			{
				speed *= 0.3f;
			}

			if ( move_dir.length_sqr( ) > 0.0001f )
			{
				move_dir = move_dir.normalized( );
				this->m_freecam_pos += move_dir * ( speed * dt );
			}
		}

		if ( std::isfinite( this->m_freecam_pos.x ) && std::isfinite( this->m_freecam_pos.y ) && std::isfinite( this->m_freecam_pos.z ) )
		{
			memory::safe_write<math::vector3>( view_setup + 0x4a0, this->m_freecam_pos );
		}
		if ( std::isfinite( this->m_freecam_angles.x ) && std::isfinite( this->m_freecam_angles.y ) && std::isfinite( this->m_freecam_angles.z ) )
		{
			memory::safe_write<math::vector3>( view_setup + 0x4b8, this->m_freecam_angles );
		}
		return true;
	}

	void camera::on_create_move( systems::input::usercmd* cmd )
	{
		if ( !cmd )
		{
			return;
		}

		this->m_cmd_buttons = cmd->buttons.value;

		const auto& cfg = settings::g_misc.m_camera;
		if ( !cfg.freecam.value )
		{
			return;
		}

		if ( !this->m_was_freecam_active || !std::isfinite( this->m_saved_viewangles.x ) || !std::isfinite( this->m_saved_viewangles.y ) || !std::isfinite( this->m_saved_viewangles.z ) )
		{
			this->m_saved_viewangles = systems::g_input.get_view_angles( );
		}

		if ( cfg.freecam_block_input.value )
		{
			const auto base = cmd->csgo_user_cmd.mutable_base( );
			if ( base )
			{
				base->set_forwardmove( 0.0f );
				base->set_leftmove( 0.0f );
				base->set_upmove( 0.0f );

				if ( const auto subticks = base->mutable_subtick_moves( ) )
				{
					subticks->clear( );
				}

				if ( base->has_viewangles( ) )
				{
					if ( const auto va = base->mutable_viewangles( ) )
					{
						va->set_x( this->m_saved_viewangles.x );
						va->set_y( this->m_saved_viewangles.y );
						va->set_z( this->m_saved_viewangles.z );
					}
				}
			}

			const auto input_history_size = cmd->csgo_user_cmd.input_history_size( );
			for ( auto i = 0; i < input_history_size; ++i )
			{
				const auto entry = cmd->csgo_user_cmd.mutable_input_history( i );
				if ( !entry )
				{
					continue;
				}

				if ( entry->has_view_angles( ) )
				{
					if ( const auto angles = entry->mutable_view_angles( ) )
					{
						angles->set_x( this->m_saved_viewangles.x );
						angles->set_y( this->m_saved_viewangles.y );
						angles->set_z( this->m_saved_viewangles.z );
					}
				}
			}

			cmd->buttons.value = 0;
			cmd->buttons.value_changed = 0;
			cmd->buttons.value_scroll = 0;
		}
	}

	void camera::reset( bool restore_view_angles )
	{
		if ( restore_view_angles && this->m_was_freecam_active )
		{
			if ( std::isfinite( this->m_saved_viewangles.x ) && std::isfinite( this->m_saved_viewangles.y ) && std::isfinite( this->m_saved_viewangles.z ) )
			{
				systems::g_input.set_view_angles( this->m_saved_viewangles );
			}
		}
		this->m_was_freecam_active = false;
		this->m_had_mouse_event = false;
		this->m_cmd_buttons = 0;
		s_was_spec_freecam = false;
		s_global_mouse_valid = false;
		s_win_cursor_valid = false;
		this->m_freecam_pos = {};
		this->m_freecam_angles = {};
		s_spec_freecam_angles = {};
		this->m_saved_viewangles = {};
		this->m_cached_fov_sensitivity = -1.0f;
		this->m_cached_scoped = false;
		this->m_cached_target_fov = 0.0f;
		s_was_spec_thirdperson = false;
		s_had_spec_mouse_event = false;
		s_spec_thirdperson_angles = {};
		s_last_spec_pawn = 0;
	}

} // namespace features::misc
