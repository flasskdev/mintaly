#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <utilities/logging/logging.hpp>
#include <core/systems/systems.hpp>
#include <core/features/features.hpp>
#include <protection/game_addresses.hpp>

namespace features::combat {

	namespace {
		[[nodiscard]] bool finite_vector( const math::vector3& value )
		{
			return std::isfinite( value.x ) && std::isfinite( value.y ) && std::isfinite( value.z );
		}

		[[nodiscard]] bool valid_trace( const systems::tracing::result& trace )
		{
			// A failed engine trace returns a zero-initialized result. Do not
			// mistake that for a collision at the world origin.
			return !trace.all_solid && std::isfinite( trace.fraction ) &&
				trace.fraction >= 0.0f && trace.fraction <= 1.0f &&
				finite_vector( trace.end_pos ) && finite_vector( trace.normal ) &&
				( trace.fraction == 1.0f || trace.normal.length_sqr( ) > 0.5f );
		}
	}

	bool shared::lagcomp::predict_movement( extrapolation_data& data, std::uintptr_t skip_entity ) const
	{
		if ( !addresses::globals::game_trace_manager || !PATTERN( patterns::trace_ray ) )
		{
			return false;
		}

		const auto gravity = CONVAR( "sv_gravity" )->get<float>( );
		if ( !std::isfinite( gravity ) || gravity < 0.0f )
		{
			return false;
		}

		const auto grounded = ( data.flags & cstypes::entity_flags::on_ground ) != 0;
		if ( grounded )
		{
			data.velocity.z = 0.0f;
		}
		else
		{
			data.velocity.z -= gravity * cstypes::tick_interval * 0.5f;
		}

		auto remaining_time = cstypes::tick_interval;
		std::array<math::vector3, 4> planes{};
		auto plane_count = 0;
		for ( auto bump = 0; bump < 4 && remaining_time > 0.0f; ++bump )
		{
			const auto end = data.origin + data.velocity * remaining_time;
			const auto trace = systems::g_tracing.trace_hull(
				data.origin, end, data.obb_mins, data.obb_maxs, skip_entity, 0x1c3003, 4 );
			if ( !valid_trace( trace ) )
			{
				return false;
			}

			data.origin = trace.end_pos;
			if ( trace.fraction == 1.0f )
			{
				remaining_time = 0.0f;
				break;
			}

			// Every bump consumes a fraction of the time left, not a fresh tick.
			remaining_time *= 1.0f - trace.fraction;
			planes[ plane_count++ ] = trace.normal.normalized( );
			const auto incoming_velocity = data.velocity;
			auto clipped = false;
			for ( auto i = 0; i < plane_count; ++i )
			{
				auto candidate = incoming_velocity;
				const auto into_plane = candidate.dot( planes[ i ] );
				if ( into_plane < 0.0f )
				{
					candidate -= planes[ i ] * into_plane;
				}

				auto enters_other_plane = false;
				for ( auto j = 0; j < plane_count; ++j )
				{
					if ( candidate.dot( planes[ j ] ) < -0.001f )
					{
						enters_other_plane = true;
						break;
					}
				}

				if ( !enters_other_plane )
				{
					data.velocity = candidate;
					clipped = true;
					break;
				}
			}

			// Complex corners need a full movement solver. Reject the speculative
			// pose rather than silently clipping through an earlier plane.
			if ( !clipped )
			{
				return false;
			}
			if ( data.velocity.length_sqr( ) < 0.000001f )
			{
				data.velocity = {};
				remaining_time = 0.0f;
				break;
			}
		}

		if ( remaining_time > 0.0f )
		{
			return false;
		}

		const auto ground_end = data.origin - math::vector3{ 0.0f, 0.0f, 2.0f };
		const auto ground_trace = systems::g_tracing.trace_hull(
			data.origin, ground_end, data.obb_mins, data.obb_maxs, skip_entity, 0x1c3003, 4 );
		if ( !valid_trace( ground_trace ) )
		{
			return false;
		}

		data.flags &= ~cstypes::entity_flags::on_ground;
		// A nearby floor must not cancel an upward jump.
		if ( data.velocity.z <= 0.0f && ground_trace.fraction < 1.0f && ground_trace.normal.z > 0.7f )
		{
			data.flags |= cstypes::entity_flags::on_ground;
			data.velocity.z = 0.0f;
		}
		else
		{
			data.velocity.z -= gravity * cstypes::tick_interval * 0.5f;
		}

		return finite_vector( data.origin ) && finite_vector( data.velocity );
	}

	std::optional<shared::lagcomp::record> shared::lagcomp::extrapolate( std::uintptr_t pawn )
	{
		if ( !settings::g_combat.m_lagcomp.extrapolation.value )
		{
			return std::nullopt;
		}

		std::shared_lock records_lock( this->m_records_mtx );
		const auto it = this->m_records.find( pawn );
		// Two observations are needed to distinguish delayed updates from the
		// normal update cadence. Never resurrect an expired pose.
		if ( it == this->m_records.end( ) || it->second.size( ) < 2 )
		{
			return std::nullopt;
		}

		const auto& latest = it->second.front( );
		const auto& previous = it->second[ 1 ];
		if ( !latest.is_valid( ) || !previous.valid || latest.extrapolated ||
			!std::isfinite( latest.simulation_time ) || !std::isfinite( previous.simulation_time ) ||
			!finite_vector( latest.origin ) || !finite_vector( previous.origin ) ||
			!finite_vector( latest.velocity ) || !finite_vector( previous.velocity ) ||
			!finite_vector( latest.obb_mins ) || !finite_vector( latest.obb_maxs ) ||
			latest.obb_maxs.x <= latest.obb_mins.x || latest.obb_maxs.y <= latest.obb_mins.y ||
			latest.obb_maxs.z <= latest.obb_mins.z )
		{
			return std::nullopt;
		}

		const auto max_ticks = std::clamp( settings::g_combat.m_lagcomp.max_extrapolate_ticks.value, 0, 16 );
		const auto dt = latest.simulation_time - previous.simulation_time;
		if ( max_ticks == 0 || dt < cstypes::tick_interval * 0.5f || dt > max_ticks * cstypes::tick_interval )
		{
			return std::nullopt;
		}

		const auto net_client = addresses::globals::network_client_service;
		if ( !net_client )
		{
			return std::nullopt;
		}
		const auto tick_state = memory::call_vfunc<std::uintptr_t>( net_client, 23 );
		if ( !tick_state )
		{
			return std::nullopt;
		}

		const auto server_tick = memory::read<int>( tick_state + 892 );
		const auto delta_ticks = static_cast<std::int64_t>( server_tick ) - latest.tick;
		const auto update_ticks = std::max( 1, cstypes::time_to_ticks( dt ) );
		if ( delta_ticks <= update_ticks || delta_ticks > max_ticks )
		{
			return std::nullopt;
		}

		// Reject discontinuities and pose transitions instead of translating an
		// old standing skeleton through a teleport, crouch, jump or landing.
		const auto pose_flags = cstypes::entity_flags::on_ground | cstypes::entity_flags::ducking;
		const auto max_distance = std::max( latest.velocity.length( ), previous.velocity.length( ) ) * dt +
			( latest.obb_maxs - latest.obb_mins ).length_2d( );
		if ( ( ( latest.flags ^ previous.flags ) & pose_flags ) != 0 ||
			( latest.obb_mins - previous.obb_mins ).length_sqr( ) > 0.01f ||
			( latest.obb_maxs - previous.obb_maxs ).length_sqr( ) > 0.01f ||
			( latest.origin - previous.origin ).length_sqr( ) > max_distance * max_distance )
		{
			return std::nullopt;
		}

		const auto speed = latest.velocity.length_2d( );
		if ( latest.velocity.length_sqr( ) < 0.01f && ( latest.flags & cstypes::entity_flags::on_ground ) )
		{
			return std::nullopt;
		}

		float direction_change = 0.0f;
		if ( speed > 0.1f && previous.velocity.length_2d( ) > 0.1f )
		{
			const auto direction = std::atan2f( latest.velocity.y, latest.velocity.x );
			const auto previous_direction = std::atan2f( previous.velocity.y, previous.velocity.x );
			const auto angle_diff = std::remainder( direction - previous_direction, 2.0f * std::numbers::pi_v<float> );
			direction_change = angle_diff * cstypes::tick_interval / dt;
			// Large turns are not evidence for continuing straight ahead.
			if ( std::fabsf( angle_diff ) > 35.0f * std::numbers::pi_v<float> / 180.0f ||
				std::fabsf( direction_change ) > 6.0f * std::numbers::pi_v<float> / 180.0f )
			{
				return std::nullopt;
			}
		}

		extrapolation_data data{};
		data.origin = latest.origin;
		data.velocity = latest.velocity;
		data.obb_mins = latest.obb_mins;
		data.obb_maxs = latest.obb_maxs;
		data.flags = latest.flags;
		data.sim_time = latest.simulation_time;

		for ( auto i = 0; i < delta_ticks; ++i )
		{
			// Rotate the current (possibly collision-clipped) velocity, not the
			// original heading that would push the player back into a wall.
			const auto x = data.velocity.x;
			const auto y = data.velocity.y;
			data.velocity.x = x * std::cosf( direction_change ) - y * std::sinf( direction_change );
			data.velocity.y = x * std::sinf( direction_change ) + y * std::cosf( direction_change );
			if ( !this->predict_movement( data, pawn ) ||
				( ( data.flags ^ latest.flags ) & cstypes::entity_flags::on_ground ) != 0 )
			{
				return std::nullopt;
			}
			data.sim_time += cstypes::tick_interval;
		}

		const auto origin_delta = data.origin - latest.origin;
		if ( origin_delta.length_sqr( ) < 0.01f )
		{
			return std::nullopt;
		}

		record extrap_record = latest;
		extrap_record.origin = data.origin;
		extrap_record.velocity = data.velocity;
		extrap_record.flags = data.flags;
		extrap_record.simulation_time = data.sim_time;
		extrap_record.tick = cstypes::time_to_ticks( data.sim_time );
		extrap_record.extrapolated = true;
		extrap_record.is_applied = false;
		for ( auto i = 0; i < extrap_record.bone_count && i < 128; ++i )
		{
			extrap_record.bones[ i ].position += origin_delta;
		}

		return extrap_record;
	}

} // namespace features::combat
