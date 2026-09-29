#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <utilities/diag.hpp>
#include <protection/game_addresses.hpp>
#include "../systems.hpp"

namespace systems {

	namespace {
		[[nodiscard]] bool valid_runtime_address( std::uintptr_t address ) noexcept
		{
			// safe_read returns an empty optional on an AV, but a field can also
			// contain the canonical sentinel -1.  Do not let either value reach
			// an engine call where it will be treated as a pointer.
			return address >= 0x10000ull &&
				address != ( std::numeric_limits<std::uintptr_t>::max )( ) &&
				address <= 0x00007FFFFFFFFFFFull;
		}

		void report_invalid_filter( )
		{
			static std::atomic_bool reported{};
			if ( !reported.exchange( true, std::memory_order_relaxed ) )
			{
				diag::write( diag::level::warning,
					"trace filter initialization produced a null vtable; tracing disabled to prevent client.dll access violation" );
			}
		}
	}

	bool tracing::is_visible( const math::vector3& start, const math::vector3& end, std::uintptr_t target_entity, std::uintptr_t skip_entity, std::uintptr_t mask ) const
	{
		if ( !addresses::globals::game_trace_manager )
		{
			return false;
		}

		if ( !std::isfinite( start.x ) || !std::isfinite( start.y ) || !std::isfinite( start.z ) ||
		     !std::isfinite( end.x ) || !std::isfinite( end.y ) || !std::isfinite( end.z ) )
		{
			return false;
		}

		const auto delta = end - start;
		const auto dist = delta.length( );
		if ( dist <= 0.001f )
		{
			return true;
		}

		const auto dir = delta / dist;
		const auto ray_start = ( skip_entity != 0 && dist > 15.0f ) ? ( start + dir * 12.0f ) : start;

		// Layer 4 is world brushes, static props, and entities; type 15 tests all brushes and static props
		const auto filter = this->make_filter( skip_entity, mask, 4, 15 );
		const auto result = this->trace( ray_start, end, filter );

		if ( result.all_solid )
		{
			return false;
		}

		if ( skip_entity && result.hit_entity == skip_entity )
		{
			return false;
		}

		if ( target_entity && result.hit_entity )
		{
			if ( result.hit_entity == target_entity )
			{
				return true;
			}

			const auto owner_handle = memory::read<std::uint32_t>( result.hit_entity + SCHEMA( "C_BaseEntity", "m_hOwnerEntity"_hash ) );
			if ( owner_handle && systems::g_entities.lookup( owner_handle ) == target_entity )
			{
				return true;
			}

			return false;
		}

		return result.fraction >= 0.98f && result.hit_entity == 0;
	}

	tracing::result tracing::trace( const math::vector3& start, const math::vector3& end, std::uintptr_t skip_entity, std::uintptr_t mask, std::uint8_t layer ) const
	{
		auto filter = this->make_filter( skip_entity, mask, layer );
		return this->trace( start, end, filter );
	}

	tracing::result tracing::trace( const math::vector3& start, const math::vector3& end, const filter& filter ) const
	{
		result result{};
		if ( !filter.valid( ) )
		{
			report_invalid_filter( );
			return result;
		}

		if ( !addresses::globals::game_trace_manager || !PATTERN( patterns::trace_ray ) )
		{
			return result;
		}

		ray ray{};

		__try
		{
			memory::call<bool>(PATTERN (patterns::trace_ray), addresses::globals::game_trace_manager, &ray, &start, &end, &filter, &result );
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
			return {};
		}

		return result;
	}

	tracing::result tracing::trace_hull( const math::vector3& start, const math::vector3& end, const math::vector3& mins, const math::vector3& maxs, std::uintptr_t skip_entity, std::uintptr_t mask, std::uint8_t layer ) const
	{
		const auto filter = this->make_filter( skip_entity, mask, layer );
		return this->trace_hull( start, end, mins, maxs, filter );
	}

	tracing::result tracing::trace_hull( const math::vector3& start, const math::vector3& end, const math::vector3& mins, const math::vector3& maxs, const filter& filter ) const
	{
		result result{};
		if ( !filter.valid( ) )
		{
			report_invalid_filter( );
			return result;
		}

		if ( !addresses::globals::game_trace_manager || !PATTERN( patterns::trace_ray ) )
		{
			return result;
		}

		ray ray{};
		ray.mins = mins;
		ray.maxs = maxs;
		ray.type = 2;

		__try
		{
			memory::call<bool>(PATTERN (patterns::trace_ray), addresses::globals::game_trace_manager, &ray, &start, &end, &filter, &result );
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
			return {};
		}

		return result;
	}

	tracing::result tracing::trace_sphere( const math::vector3& start, const math::vector3& end, float radius, const filter& filter ) const
	{
		result result{};
		if ( !filter.valid( ) )
		{
			report_invalid_filter( );
			return result;
		}

		if ( !addresses::globals::game_trace_manager || !PATTERN( patterns::trace_ray ) )
		{
			return result;
		}

		ray ray{};
		ray.mins = {};
		*reinterpret_cast< float* >( reinterpret_cast< std::uintptr_t >( &ray ) + 12 ) = radius;
		ray.type = 1;

		__try
		{
			memory::call<bool>(PATTERN (patterns::trace_ray), addresses::globals::game_trace_manager, &ray, &start, &end, &filter, &result );
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
			return {};
		}

		return result;
	}

	tracing::result tracing::trace_to_entity( const math::vector3& start, const math::vector3& end, std::uintptr_t target_entity, std::uintptr_t skip_entity, std::uintptr_t mask, std::uint8_t layer ) const
	{
		const auto filter = this->make_filter( skip_entity, mask, layer );
		return this->trace_to_entity( start, end, target_entity, filter );
	}

	tracing::result tracing::trace_to_entity( const math::vector3& start, const math::vector3& end, std::uintptr_t target_entity, const filter& filter ) const
	{
		result result{};
		if ( !filter.valid( ) )
		{
			report_invalid_filter( );
			return result;
		}

		if ( !addresses::globals::game_trace_manager || !PATTERN( patterns::trace_ray_entity ) )
		{
			return result;
		}

		ray ray{};

		__try
		{
			memory::call<bool>(PATTERN (patterns::trace_ray_entity), addresses::globals::game_trace_manager, &ray, &start, &end, target_entity, &filter, &result );
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
			return {};
		}

		return result;
	}

	tracing::filter tracing::make_filter( std::uintptr_t skip_entity, std::uintptr_t mask, std::uint8_t layer, int type ) const
	{
		filter filter{};
		if ( !PATTERN( patterns::trace_filter_init ) )
		{
			return filter;
		}

		__try
		{
			memory::call<void>(PATTERN (patterns::trace_filter_init), &filter, skip_entity, mask, layer, type );
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
		}

		if ( !filter.valid( ) )
		{
			report_invalid_filter( );
		}

		return filter;
	}

	tracing::filter tracing::make_filter( std::uintptr_t skip_entity, std::uintptr_t mask, std::uint8_t layer ) const
	{
		filter filter{};
		if ( !PATTERN( patterns::trace_filter_init ) )
		{
			return filter;
		}

		__try
		{
			memory::call<void>(PATTERN (patterns::trace_filter_init), &filter, skip_entity, mask, layer, 7 );
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
		}

		if ( !filter.valid( ) )
		{
			report_invalid_filter( );
		}

		return filter;
	}

	tracing::player_movement_filter tracing::make_player_movement_filter( std::uintptr_t entity, std::uintptr_t mask, std::uint8_t collision_group ) const
	{
		player_movement_filter filter{};
		const auto init = PATTERN( patterns::trace_filter_set_collision );
		if ( !init || !valid_runtime_address( entity ) )
		{
			return filter;
		}

		__try
		{
			memory::call<void>( init, &filter, entity, mask, static_cast< int >( collision_group ) );
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
			return {};
		}

		return filter;
	}

	tracing::result tracing::trace_player_bbox( const math::vector3& start, const math::vector3& end, const bbox_collision& bbox, const player_movement_filter& filter, std::uintptr_t movement_services ) const
	{
		result result{};
		const auto trace_hull = PATTERN( patterns::trace_hull );
		if ( !trace_hull || !valid_runtime_address( movement_services ) ||
			!std::isfinite( start.x ) || !std::isfinite( start.y ) || !std::isfinite( start.z ) ||
			!std::isfinite( end.x ) || !std::isfinite( end.y ) || !std::isfinite( end.z ) )
		{
			return result;
		}

		// The player filter is an opaque engine object.  A failed initializer
		// leaves it zeroed; passing that object into trace_hull is not safe.
		const auto filter_word = *reinterpret_cast<const std::uintptr_t*>( filter.data );
		if ( !valid_runtime_address( filter_word ) )
		{
			return result;
		}

		__try
		{
			memory::call<void>( trace_hull, movement_services + 1592, &result, &start, &end, &bbox, &filter );
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
			return {};
		}

		return result;
	}

	void tracing::setup_trace( trace_data* trace_data, const math::vector3& start, const math::vector3& delta, const filter& filter, int penetration_count, bool trace_world ) const
	{
		if ( !trace_data || !filter.valid( ) )
		{
			if ( !filter.valid( ) ) report_invalid_filter( );
			return;
		}

		memory::call<void>(PATTERN (patterns::trace_bullet_data_init), trace_data, start, delta, filter, penetration_count, trace_world );
	}

	void tracing::init_result( result* trace_result ) const
	{
		memory::call<void>(PATTERN (patterns::trace_bullet_free), trace_result );
	}

	void tracing::finalize_trace( trace_data* trace_data, result* hit, float unknown_float, void* unknown ) const
	{
		memory::call<void>(PATTERN (patterns::trace_bullet_update), trace_data, hit, unknown_float, unknown );
	}

} // namespace systems
