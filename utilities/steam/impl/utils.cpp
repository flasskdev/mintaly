#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <protection/game_addresses.hpp>
#include "../steam.hpp"

namespace steam {

	namespace detail {

		inline std::uintptr_t utils_interface {};

	} // namespace detail

	bool utils::initialize () {
		if ( !detail::utils_interface ) {
			const auto export_addr = MODULE_EXPORT ("steam_api64.dll:SteamAPI_SteamUtils_v010");
			if ( export_addr ) {
				detail::utils_interface = memory::safe_call<std::uintptr_t> (export_addr);
			}
		}
		return detail::utils_interface != 0;
	}

	bool utils::get_image_size (int image, std::uint32_t* width, std::uint32_t* height) {
		if (image <= 0 || !initialize() || !width || !height) {
			return false;
		}

		const auto fn = MODULE_EXPORT ("steam_api64.dll:SteamAPI_ISteamUtils_GetImageSize");
		if (!fn) return false;
		return memory::safe_call<bool> (fn, detail::utils_interface, image, width, height);
	}

	bool utils::get_image_rgba (int image, std::uint8_t* dest, int dest_size) {
		if (image <= 0 || !initialize() || !dest || dest_size <= 0) {
			return false;
		}

		const auto fn = MODULE_EXPORT ("steam_api64.dll:SteamAPI_ISteamUtils_GetImageRGBA");
		if (!fn) return false;
		return memory::safe_call<bool> (fn, detail::utils_interface, image, dest, dest_size);
	}

} // namespace steam