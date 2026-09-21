#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <utilities/logging/logging.hpp>
#include <core/systems/systems.hpp>
#include <core/rendering/rendering.hpp>
#include <core/settings.hpp>
#include <protection/game_addresses.hpp>
#include "../world.hpp"

namespace features::world {
	namespace {
		struct particle_transform {
			float px{};
			float py{};
			float pz{};
			float pw{};
			float qx{};
			float qy{};
			float qz{};
			float qw{ 1.0f };
		};
		static_assert (sizeof (particle_transform) == 0x20);
	}

	void weather::on_frame_stage_notify( )
	{
		const auto manager = memory::safe_read<std::uintptr_t>( addresses::globals::particle_manager ).value_or( 0 );
		if ( manager != this->m_particle_manager )
		{
			// Handles belong to their original manager; never destroy them through a replacement.
			this->m_effect_index = invalid_effect_index;
			this->m_last_particle_type = -1;
			this->m_last_round_start_time = 0.0f;
			this->m_particle_loaded = false;
			this->m_color_initialized = false;
			this->m_last_origin = {};
			this->m_particle_manager = manager;
		}

		if ( !settings::g_world.m_weather.enabled.value )
		{
			if ( this->m_effect_index != invalid_effect_index )
			{
				this->release_particles( );
			}

			this->m_last_particle_type = -1;
			return;
		}

		const auto game_rules = memory::read<std::uintptr_t>( addresses::globals::game_rules );
		if ( !game_rules )
		{
			this->release_particles( );
			return;
		}

		const auto round_start_time = memory::read<float>( game_rules + SCHEMA( "C_CSGameRules", "m_fRoundStartTime"_hash ) );
		if ( round_start_time > 0.0f && round_start_time != this->m_last_round_start_time )
		{
			if ( this->m_last_round_start_time != 0.0f )
			{
				this->release_particles( );
			}
			this->m_last_round_start_time = round_start_time;
		}

		this->update_particles( );
	}

	void weather::release( bool destroy_effect )
	{
		if ( destroy_effect )
		{
			this->release_particles( );
		}
		this->m_effect_index = invalid_effect_index;
		this->m_last_particle_type = -1;
		this->m_last_round_start_time = 0.0f;
		this->m_particle_loaded = false;
		this->m_color_initialized = false;
		this->m_last_origin = {};
	}

	void weather::create_particle( )
	{
		const auto particle_manager = memory::read<std::uintptr_t>( addresses::globals::particle_manager );
		if ( !particle_manager || !addresses::globals::resource_system )
		{
			return;
		}
		if ( !PATTERN( patterns::init_particle_path_buffer_alt )
			|| !PATTERN( patterns::resource_system_load )
			|| !PATTERN( patterns::particle_create_effect )
			|| !PATTERN( patterns::particle_set_control_point ) )
		{
			static bool reported{};
			if ( !reported )
			{
				logging::console::print( "[weather] required particle helper unavailable; check signatures for this game build" );
				reported = true;
			}
			return;
		}

		std::string particle_path = "";

		switch ( settings::g_world.m_weather.type )
		{
		case settings::world::weather::weather_type::snow:
			particle_path = xs( "particles/embedded/snow.vpcf" );
			break;
		case settings::world::weather::weather_type::rain:
			particle_path = xs( "particles/embedded/rain.vpcf" );
			break;
		case settings::world::weather::weather_type::stars:
			particle_path = xs( "particles/embedded/stars.vpcf" );
			break;
		default:
			return;
		}

		if ( this->m_effect_index != invalid_effect_index )
		{
			this->release_particles( );
		}

		struct buffer_string
		{
			std::uint32_t m_unknown1{};
			std::uint32_t m_unknown2{ 0xc00000c8 };

			union
			{
				std::uintptr_t m_str_ptr;
				std::uint8_t data[ 0xc8 ];
			};

			std::uintptr_t m_unknown3{};
			std::uintptr_t m_unknown4{};
		} buffer{};

		memory::call<void>( PATTERN (patterns::init_particle_path_buffer_alt), &buffer, particle_path.c_str( ) );
		buffer.m_unknown4 = 'fcpv';
		memory::call<void>( PATTERN (patterns::resource_system_load), addresses::globals::resource_system, &buffer, "" );

		auto effect_index{ invalid_effect_index };
		if ( PATTERN (patterns::particle_create_effect) )
		{
			memory::call<int*>( PATTERN (patterns::particle_create_effect), particle_manager, &effect_index, particle_path.c_str( ), 8, 0ll, 0ll, 0ll, 0 );
		}

		this->m_effect_index = effect_index;
		this->m_last_particle_type = static_cast< int >( settings::g_world.m_weather.type.value );
		static bool creation_failed{};
		if ( effect_index == invalid_effect_index && !creation_failed )
			logging::console::print( "[weather] effect creation failed; check embedded particle loading" );
		creation_failed = effect_index == invalid_effect_index;
	}

	void weather::update_particles( )
	{
		// Do not spawn at the world origin while the observer/player model is unavailable.
		const auto pawn = systems::g_local.get( ).view_pawn( );
		if ( !pawn || !memory::safe_read<std::uintptr_t>( pawn + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) ).value_or( 0 ) )
			return;
		const auto current_type = static_cast< int >( settings::g_world.m_weather.type.value );

		if ( this->m_effect_index != invalid_effect_index && this->m_last_particle_type != current_type )
		{
			this->release_particles( );
		}

		if ( this->m_effect_index == invalid_effect_index )
		{
			static auto s_last_create_attempt = std::chrono::steady_clock::time_point{};
			const auto now = std::chrono::steady_clock::now( );
			if ( std::chrono::duration<float>( now - s_last_create_attempt ).count( ) < 0.25f )
			{
				return;
			}
			s_last_create_attempt = now;

			this->create_particle( );

			if ( this->m_effect_index == invalid_effect_index )
			{
				return;
			}
		}

		// Publish control points every update, including resource initialization frames.
		const auto particle_manager = memory::read<std::uintptr_t>( addresses::globals::particle_manager );
		const auto view_pawn = systems::g_local.get( ).view_pawn( );

		if ( !view_pawn || !particle_manager )
		{
			return;
		}

		const auto game_scene_node = memory::read<std::uintptr_t>( view_pawn + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) );
		if ( !game_scene_node )
		{
			return;
		}

		const auto origin = memory::read<math::vector3>( game_scene_node + SCHEMA( "CGameSceneNode", "m_vecAbsOrigin"_hash ) );
		const auto& weather = settings::g_world.m_weather;

		const auto now = std::chrono::steady_clock::now( );
		const auto time_since_last_update = std::chrono::duration<float, std::milli>( now - this->m_last_update_time ).count( );
		const bool origin_changed = ( origin - this->m_last_origin ).length_sqr( ) > 9.0f;
		const bool is_rain_wind = ( weather.type.value == settings::world::weather::weather_type::rain && weather.wind.value );

		// Throttle particle control point updates: at 200+ FPS, calling particle_set_control_point
		// every frame invalidates engine emitter bounds and causes heavy FPS drops.
		// Update at most ~60 Hz (>= 16ms) or when the player has meaningfully moved.
		if ( origin_changed || time_since_last_update >= 16.0f || !this->m_color_initialized )
		{
			this->m_last_update_time = now;
			this->m_last_origin = origin;

			if ( is_rain_wind )
			{
				const auto direction = weather.wind_direction.value * ( std::numbers::pi_v<float> / 180.0f );
				const auto strength = std::clamp( weather.wind_strength.value / 5.0f, 0.0f, 1.0f );
				const auto turbulence = std::clamp( weather.wind_turbulence.value / 5.0f, 0.0f, 1.0f );
				float vx = strength * std::cos( direction );
				float vy = strength * std::sin( direction );

				if ( turbulence > 0.0f )
				{
					const auto time = static_cast<double>( GetTickCount64( ) ) * 0.0006;
					const auto noise_x = static_cast<float>(
						0.60 * std::sin( time * 0.90 + 0.3 ) +
						0.30 * std::sin( time * 2.30 + 1.7 ) +
						0.15 * std::sin( time * 5.10 + 4.2 ) );
					const auto noise_y = static_cast<float>(
						0.60 * std::sin( time * 1.10 + 2.0 ) +
						0.30 * std::sin( time * 2.70 + 0.5 ) +
						0.15 * std::sin( time * 4.60 + 3.1 ) );
					vx += turbulence * 0.9f * noise_x;
					vy += turbulence * 0.9f * noise_y;
				}

				const auto magnitude = std::min( std::sqrt( vx * vx + vy * vy ), 1.0f );
				const auto tilt = magnitude * 80.0f * ( std::numbers::pi_v<float> / 180.0f );
				const auto heading = vx != 0.0f || vy != 0.0f ? std::atan2( vy, vx ) : 0.0f;
				const auto sine = std::sin( tilt * 0.5f );
				const particle_transform transform{
					origin.x, origin.y, origin.z, 0.0f,
					std::sin( heading ) * sine,
					-std::cos( heading ) * sine,
					0.0f,
					std::cos( tilt * 0.5f )
				};
				memory::call<bool>( PATTERN( patterns::particle_set_transform ), particle_manager, this->m_effect_index, 0, &transform, 0 );
			}
			else
			{
				memory::call<bool>( PATTERN( patterns::particle_set_control_point ), particle_manager, this->m_effect_index, 0, &origin, 0 );
			}
		}

		// Only push color when changed or uninitialized (avoids marking CP 1 dirty every single frame).
		const auto color = math::vector3{ static_cast< float >( weather.color.value.r ), static_cast< float >( weather.color.value.g ), static_cast< float >( weather.color.value.b ) };
		if ( !this->m_color_initialized || color.x != this->m_last_color.x || color.y != this->m_last_color.y || color.z != this->m_last_color.z )
		{
			memory::call<void>( PATTERN( patterns::particle_set_control_point ), particle_manager, this->m_effect_index, 1, &color, 0 );
			this->m_last_color = color;
			this->m_color_initialized = true;
		}
	}

	void weather::release_particles( )
	{
		if ( this->m_effect_index == invalid_effect_index )
		{
			return;
		}

		const auto particle_manager = memory::safe_read<std::uintptr_t>( addresses::globals::particle_manager ).value_or( 0 );
		if ( particle_manager && PATTERN( patterns::particle_destroy_effect ) )
		{
			memory::call<void>( PATTERN( patterns::particle_destroy_effect ), particle_manager, this->m_effect_index, true, true );
		}

		this->m_effect_index = invalid_effect_index;
		this->m_last_particle_type = -1;
		this->m_particle_loaded = false;
		this->m_color_initialized = false;
		this->m_last_origin = {};
	}

} // namespace features::world
