#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>

#include "../systems.hpp"

namespace systems {

	std::uint32_t schemas::lookup( const char* class_name, std::uint32_t field_hash )
	{
		const auto type_scope = memory::call_vfunc<std::uintptr_t>( addresses::globals::schema_system, 13, xs( "client.dll" ), nullptr );
		if ( !type_scope )
		{
			return 0;
		}

		auto class_info{ 0ull };
		memory::call_vfunc<void>( type_scope, 2, &class_info, class_name );

		if ( !class_info )
		{
			return 0;
		}

		const auto fields_ptr = memory::read<std::uintptr_t>( class_info + 0x30 );
		const auto field_count = memory::read<std::uint16_t>( class_info + 0x24 );

		if ( !fields_ptr || !field_count )
		{
			return 0;
		}

		for ( std::uint16_t i = 0; i < field_count; ++i )
		{
			const auto field_addr = fields_ptr + ( static_cast< std::size_t >( i ) * 0x20 );
			const auto name_ptr = memory::read<std::uintptr_t>( field_addr );

			if ( !name_ptr )
			{
				continue;
			}

			if ( fnv1a::runtime_hash( reinterpret_cast< const char* >( name_ptr ) ) == field_hash )
			{
				return memory::read<std::uint32_t>( field_addr + 0x10 );
			}
		}

		return 0;
	}

	void schemas::dump_fields( const char* class_name )
	{
		if ( !addresses::globals::schema_system || !class_name ) return;
		const auto scope = memory::call_vfunc<std::uintptr_t>( addresses::globals::schema_system, 13, xs( "client.dll" ), nullptr );
		if ( !scope ) return;
		std::uintptr_t info{};
		memory::call_vfunc<void>( scope, 2, &info, class_name );
		if ( !info )
		{
			diag::writef( diag::level::warning, "[skin-schema] class=%s unavailable", class_name );
			return;
		}
		// Same declared-field layout as lookup(); do not guess base-class metadata.
		const auto fields = memory::safe_read<std::uintptr_t>( info + 0x30 ).value_or( 0 );
		const auto count = memory::safe_read<std::uint16_t>( info + 0x24 );
		if ( !count || *count > 256 || ( *count && !fields ) ) return;
		diag::writef( diag::level::info, "[skin-schema] class=%s fields=%u", class_name, static_cast<unsigned>( *count ) );
		for ( std::uint16_t i = 0; i < *count; ++i )
		{
			const auto field = fields + static_cast<std::size_t>( i ) * 0x20;
			const auto name = memory::safe_read<std::uintptr_t>( field ).value_or( 0 );
			const auto offset = memory::safe_read<std::uint32_t>( field + 0x10 );
			if ( !name || !offset ) continue;
			const auto text = memory::read_string( name, 128 );
			diag::writef( diag::level::info, "[skin-schema] class=%s field=%s offset=%u", class_name, text.c_str( ), static_cast<unsigned>( *offset ) );
		}
	}

} // namespace systems