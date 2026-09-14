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
		detail::user_interface = memory::call<std::uintptr_t>( MODULE_EXPORT( "steam_api64.dll:SteamAPI_SteamUser_v023" ) );
		return detail::user_interface != 0;
	}

	std::uint64_t user::get_steam_id( )
	{
		// Optional Steam initialization may fail in development builds. A valid
		// export still must not be called with a null ISteamUser instance.
		if ( !detail::user_interface )
			return 0;
		return memory::call<std::uint64_t>( MODULE_EXPORT( "steam_api64.dll:SteamAPI_ISteamUser_GetSteamID" ), detail::user_interface );
	}

} // namespace steam
