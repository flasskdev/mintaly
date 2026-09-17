#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <protection/game_addresses.hpp>
#include "../steam.hpp"

namespace steam {

	namespace detail {

		inline std::uintptr_t user_interface{};

	} // namespace detail

	bool user::initialize( )
	{
		if ( !detail::user_interface ) {
			const auto export_addr = MODULE_EXPORT( "steam_api64.dll:SteamAPI_SteamUser_v023" );
			if ( export_addr ) {
				detail::user_interface = memory::safe_call<std::uintptr_t>( export_addr );
			}
		}
		return detail::user_interface != 0;
	}

	std::uint64_t user::get_steam_id( )
	{
		if ( !initialize( ) )
			return 0;
		const auto fn = MODULE_EXPORT( "steam_api64.dll:SteamAPI_ISteamUser_GetSteamID" );
		if ( !fn ) return 0;
		return memory::safe_call<std::uint64_t>( fn, detail::user_interface );
	}

} // namespace steam
