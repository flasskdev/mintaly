#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <utilities/mvp_music.hpp>
#include <utilities/lifecycle.hpp>
#include <core/systems/systems.hpp>
#include <core/features/features.hpp>
#include <core/settings.hpp>
#include <protection/game_addresses.hpp>

namespace features::changer {
namespace {
	std::mutex s_mvp_mutex;
	mvp_music::resolver s_mvp;
	DWORD s_music_thread{};
}

	void music::on_frame_stage_notify( )
	{
		const auto local = systems::g_local.get( );
		if ( !local.controller || lifecycle::is_unloading( ) )
		{
			this->clear_mvp( );
			return;
		}

		std::optional<mvp_music::resolver::playback> pending;
		{
			std::lock_guard lock( s_mvp_mutex );
			const auto thread = GetCurrentThreadId( );
			if ( s_music_thread && s_music_thread != thread ) s_mvp.clear( );
			s_music_thread = thread;
			pending = s_mvp.poll( std::chrono::steady_clock::now( ) );
		}
		// The engine may re-enter hooks. Never invoke it with the state mutex held.
		if ( pending ) pending->play( pending->kit );

		const auto controller = local.controller;
		const auto inv_services_offset = SCHEMA( "CCSPlayerController", "m_pInventoryServices"_hash );
		const auto inventory_services = inv_services_offset ? memory::read<std::uintptr_t>( controller + inv_services_offset ) : 0;
		const auto music_id_offset = SCHEMA( "CCSPlayerController_InventoryServices", "m_unMusicID"_hash );
		const auto kit_id_offset = SCHEMA( "CCSPlayerController", "m_iMusicKitID"_hash );
		const auto mvp_no_music_offset = SCHEMA( "CCSPlayerController", "m_bMvpNoMusic"_hash );
		const auto kit_mvps_offset = SCHEMA( "CCSPlayerController", "m_iMusicKitMVPs"_hash );

		if ( this->m_last_controller != controller )
		{
			this->m_captured = false;
			this->m_last_controller = controller;
		}
		if ( !this->m_captured )
		{
			if ( inventory_services && music_id_offset )
				this->m_original_music = memory::read<std::uint16_t>( inventory_services + music_id_offset );
			if ( kit_id_offset )
				this->m_original_music_kit_id = memory::read<std::int32_t>( controller + kit_id_offset );
			if ( mvp_no_music_offset )
				this->m_original_mvp_no_music = memory::read<bool>( controller + mvp_no_music_offset );
			if ( kit_mvps_offset )
				this->m_original_music_kit_mvps = memory::read<std::int32_t>( controller + kit_mvps_offset );
			this->m_captured = true;
		}

		const auto configured_id = settings::g_changer.music.id;
		const auto target_id = mvp_music::valid_kit( configured_id ) ? static_cast<std::uint16_t>( configured_id ) : 0;
		if ( target_id > 0 )
		{
			if ( inventory_services && music_id_offset && memory::read<std::uint16_t>( inventory_services + music_id_offset ) != target_id )
				memory::write<std::uint16_t>( inventory_services + music_id_offset, target_id );
			if ( kit_id_offset && memory::read<std::int32_t>( controller + kit_id_offset ) != target_id )
				memory::write<std::int32_t>( controller + kit_id_offset, target_id );
			if ( mvp_no_music_offset && memory::read<bool>( controller + mvp_no_music_offset ) )
				memory::write<bool>( controller + mvp_no_music_offset, false );
			if ( kit_mvps_offset && memory::read<std::int32_t>( controller + kit_mvps_offset ) < 1 )
				memory::write<std::int32_t>( controller + kit_mvps_offset, 1 );
		}
		else if ( this->m_captured )
		{
			if ( inventory_services && music_id_offset && memory::read<std::uint16_t>( inventory_services + music_id_offset ) != this->m_original_music )
				memory::write<std::uint16_t>( inventory_services + music_id_offset, this->m_original_music );
			if ( kit_id_offset && memory::read<std::int32_t>( controller + kit_id_offset ) != this->m_original_music_kit_id )
				memory::write<std::int32_t>( controller + kit_id_offset, this->m_original_music_kit_id );
			if ( mvp_no_music_offset )
				memory::write<bool>( controller + mvp_no_music_offset, this->m_original_mvp_no_music );
			if ( kit_mvps_offset )
				memory::write<std::int32_t>( controller + kit_mvps_offset, this->m_original_music_kit_mvps );
		}
	}

	void music::reset( )
	{
		// Discard captured engine callbacks before any map-owned objects are touched.
		this->clear_mvp( );
		{
			std::lock_guard lock( s_mvp_mutex );
			s_music_thread = 0;
		}
		// During cache-only teardown the local snapshot is already reset: do not
		// restore fields through the previous map's stale controller pointer.
		if ( this->m_captured && this->m_last_controller && systems::g_local.get( ).controller == this->m_last_controller )
		{
			const auto inv_services_offset = SCHEMA( "CCSPlayerController", "m_pInventoryServices"_hash );
			if ( inv_services_offset )
			{
				const auto inventory_services = memory::safe_read<std::uintptr_t>( this->m_last_controller + inv_services_offset ).value_or( 0 );
				const auto music_id_offset = SCHEMA( "CCSPlayerController_InventoryServices", "m_unMusicID"_hash );
				if ( inventory_services && music_id_offset )
					memory::safe_write<std::uint16_t>( inventory_services + music_id_offset, this->m_original_music );
			}
			const auto kit_id_offset = SCHEMA( "CCSPlayerController", "m_iMusicKitID"_hash );
			if ( kit_id_offset )
				memory::safe_write<std::int32_t>( this->m_last_controller + kit_id_offset, this->m_original_music_kit_id );
			const auto mvp_no_music_offset = SCHEMA( "CCSPlayerController", "m_bMvpNoMusic"_hash );
			if ( mvp_no_music_offset )
				memory::safe_write<bool>( this->m_last_controller + mvp_no_music_offset, this->m_original_mvp_no_music );
			const auto kit_mvps_offset = SCHEMA( "CCSPlayerController", "m_iMusicKitMVPs"_hash );
			if ( kit_mvps_offset )
				memory::safe_write<std::int32_t>( this->m_last_controller + kit_mvps_offset, this->m_original_music_kit_mvps );
		}
		this->m_original_music = 0;
		this->m_original_music_kit_id = 0;
		this->m_original_mvp_no_music = false;
		this->m_original_music_kit_mvps = 0;
		this->m_captured = false;
		this->m_last_controller = 0;
	}

	void music::clear_mvp( )
	{
		std::lock_guard lock( s_mvp_mutex );
		s_mvp.clear( );
		this->m_local_won_last_mvp = false;
		this->m_last_mvp_time = {};
	}

	bool music::queue_mvp_music( void* context, int track, std::uint16_t kit, float volume,
		void (*play)( void*, int, std::uint16_t, float ) )
	{
		if ( !context || !play || track != mvp_music::anthem_track || lifecycle::is_unloading( ) ) return false;
		std::optional<mvp_music::resolver::playback> immediate;
		{
			std::lock_guard lock( s_mvp_mutex );
			// Preserve thread affinity: do not move an audio-thread callback to the
			// game thread. Unknown execution contexts keep the original engine path.
			if ( !s_music_thread || s_music_thread != GetCurrentThreadId( ) ) return false;
			immediate = s_mvp.submit( kit, [context, track, volume, play]( std::uint16_t selected ) {
				if ( !lifecycle::is_unloading( ) ) play( context, track, selected, volume );
			}, std::chrono::steady_clock::now( ) );
		}
		if ( immediate ) immediate->play( immediate->kit );
		return true;
	}

	void music::on_round_mvp( void* event )
	{
		if ( !event ) return;
		// Do not clear here: an engine listener can request playback BEFORE this
		// listener runs. That pending request must survive until the next frame.
		const auto mvp_controller = systems::events::get_controller( event, "userid" );
		const auto local_controller = systems::g_local.get( ).controller;
		const bool local_winner = mvp_controller && mvp_controller == local_controller;
		int target_id = 0;
		if ( local_winner ) target_id = settings::g_changer.music.id;
		else if ( mvp_controller )
		{
			const auto offset = SCHEMA( "CBasePlayerController", "m_steamID"_hash );
			const auto steam_id = offset ? memory::safe_read<std::uint64_t>( mvp_controller + offset ).value_or( 0 ) : 0;
			if ( steam_id ) target_id = g_skin_sync.get_remote_music_kit( steam_id );
		}
		if ( !mvp_music::valid_kit( target_id ) ) target_id = 0;

		int winner_kit = target_id;
		if ( !winner_kit && mvp_controller )
		{
			const auto offset = SCHEMA( "CCSPlayerController", "m_iMusicKitID"_hash );
			if ( offset ) winner_kit = memory::safe_read<int>( mvp_controller + offset ).value_or( 0 );
			if ( !mvp_music::valid_kit( winner_kit ) )
			{
				const auto services_offset = SCHEMA( "CCSPlayerController", "m_pInventoryServices"_hash );
				const auto services = services_offset ? memory::safe_read<std::uintptr_t>( mvp_controller + services_offset ).value_or( 0 ) : 0;
				const auto music_offset = SCHEMA( "CCSPlayerController_InventoryServices", "m_unMusicID"_hash );
				if ( services && music_offset ) winner_kit = memory::safe_read<std::uint16_t>( services + music_offset ).value_or( 0 );
			}
		}
		{
			std::lock_guard lock( s_mvp_mutex );
			const auto now = std::chrono::steady_clock::now( );
			s_mvp.set_winner( winner_kit, now );
			this->m_local_won_last_mvp = local_winner;
			this->m_last_mvp_time = now;
		}

		if ( target_id > 0 && mvp_controller )
		{
			const auto kit_id_offset = SCHEMA( "CCSPlayerController", "m_iMusicKitID"_hash );
			if ( kit_id_offset ) memory::safe_write<std::int32_t>( mvp_controller + kit_id_offset, target_id );
			const auto mvp_no_music_offset = SCHEMA( "CCSPlayerController", "m_bMvpNoMusic"_hash );
			if ( mvp_no_music_offset ) memory::safe_write<bool>( mvp_controller + mvp_no_music_offset, false );
			const auto services_offset = SCHEMA( "CCSPlayerController", "m_pInventoryServices"_hash );
			const auto services = services_offset ? memory::safe_read<std::uintptr_t>( mvp_controller + services_offset ).value_or( 0 ) : 0;
			const auto music_offset = SCHEMA( "CCSPlayerController_InventoryServices", "m_unMusicID"_hash );
			if ( services && music_offset ) memory::safe_write<std::uint16_t>( services + music_offset, static_cast<std::uint16_t>( target_id ) );
		}
	}

	bool music::is_local_mvp( ) const
	{
		std::lock_guard lock( s_mvp_mutex );
		return this->m_local_won_last_mvp &&
			std::chrono::steady_clock::now( ) - this->m_last_mvp_time <= std::chrono::seconds( 20 );
	}

	int get_current_mvp_kit_id( )
	{
		std::lock_guard lock( s_mvp_mutex );
		return s_mvp.winner_kit( std::chrono::steady_clock::now( ) );
	}
}
