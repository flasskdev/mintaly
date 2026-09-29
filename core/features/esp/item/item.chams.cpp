#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <utilities/diag.hpp>
#include <core/systems/systems.hpp>
#include <core/settings.hpp>
#include <core/features/features.hpp>
#include "../primitive_buffer.hpp"
#include "../weapon_definition.hpp"

namespace features::esp::item {

	namespace {
		[[nodiscard]] bool is_glow_material( settings::esp::cham_ids id ) noexcept
		{
			return id == settings::esp::cham_ids::glow || id == settings::esp::cham_ids::glow_ignorez ||
				id == settings::esp::cham_ids::outline_glow || id == settings::esp::cham_ids::outline_glow_ignorez;
		}

		[[nodiscard]] bool is_outline_glow_material( settings::esp::cham_ids id ) noexcept
		{
			return id == settings::esp::cham_ids::outline_glow || id == settings::esp::cham_ids::outline_glow_ignorez;
		}

		void apply_outline_glow_pulse( xdraw::color& color, settings::esp::cham_ids id,
			const settings::esp::outline_glow_config* glow_cfg )
		{
			if ( !is_outline_glow_material( id ) )
				return;

			const auto pulse = glow_cfg ? glow_cfg->pulse_speed.value : settings::g_esp.m_outline_glow.pulse_speed.value;
			if ( pulse <= 0.01f )
				return;

			const auto now = std::chrono::steady_clock::now( );
			const auto sec = std::chrono::duration<float>( now.time_since_epoch( ) ).count( );
			const auto wave = 0.5f + 0.5f * std::sin( sec * pulse * 4.0f );
			const auto scale = 0.3f + 0.7f * wave;
			color.r = static_cast<std::uint8_t>( color.r * scale );
			color.g = static_cast<std::uint8_t>( color.g * scale );
			color.b = static_cast<std::uint8_t>( color.b * scale );
			color.a = static_cast<std::uint8_t>( color.a * scale );
		}

		void keep_glow_color_visible( xdraw::color& color ) noexcept
		{
			const auto peak = ( std::max )( color.r, ( std::max )( color.g, color.b ) );
			if ( peak <= 12 )
				return;

			if ( peak < 144 )
			{
				const auto gain = 144.0f / static_cast<float>( peak );
				color.r = static_cast<std::uint8_t>( ( std::min )( 255.0f, color.r * gain ) );
				color.g = static_cast<std::uint8_t>( ( std::min )( 255.0f, color.g * gain ) );
				color.b = static_cast<std::uint8_t>( ( std::min )( 255.0f, color.b * gain ) );
			}
			color.a = ( std::max )( color.a, static_cast<std::uint8_t>( 160 ) );
		}

		[[nodiscard]] bool is_ignorez_material( settings::esp::cham_ids id ) noexcept
		{
			return id == settings::esp::cham_ids::liquid_ignorez || id == settings::esp::cham_ids::matte_ignorez ||
				id == settings::esp::cham_ids::flat_ignorez || id == settings::esp::cham_ids::bloom_ignorez ||
				id == settings::esp::cham_ids::outlines_ignorez || id == settings::esp::cham_ids::glow_ignorez ||
				id == settings::esp::cham_ids::distortion_ignorez || id == settings::esp::cham_ids::hologram_ignorez ||
				id == settings::esp::cham_ids::glass_ignorez || id == settings::esp::cham_ids::crystal_ignorez ||
				id == settings::esp::cham_ids::pulse_ignorez || id == settings::esp::cham_ids::animated_ignorez ||
				id == settings::esp::cham_ids::rainbow_ignorez || id == settings::esp::cham_ids::fresnel_ignorez ||
				id == settings::esp::cham_ids::iridescent_ignorez || id == settings::esp::cham_ids::self_illum_ignorez ||
				id == settings::esp::cham_ids::wireframe_ignorez || id == settings::esp::cham_ids::velvet_ignorez ||
				id == settings::esp::cham_ids::chrome_ignorez || id == settings::esp::cham_ids::gold_ignorez ||
				id == settings::esp::cham_ids::outline_glow_ignorez;
		}
	}

	bool chams::on_generate_primitives( std::uintptr_t owner_entity, std::uint32_t owner_hash, std::uintptr_t scene_object, std::uintptr_t primitive_buffer, std::uintptr_t( __fastcall* original_fn )( std::uintptr_t, std::uintptr_t, std::uintptr_t, std::uintptr_t ), std::uintptr_t a1, std::uintptr_t scene_view )
	{
		if ( systems::g_model_preview.item_preview_active( ) )
			return false;

		const auto& chams_cfg = settings::g_esp.m_item.m_chams;
		if ( !chams_cfg.enabled.value )
		{
			return false;
		}

		const auto group_id = this->get_item_group( owner_hash );
		if ( group_id == UINT32_MAX ) return false;
		const auto* specific = chams_cfg.individual.find( detail::weapon_definition( owner_entity ) );
		if ( !specific && !chams_cfg.is_active( group_id ) ) return false;

		const auto& cfg = specific ? *specific : chams_cfg.get_group( group_id );
		if ( !cfg.enabled.value || ( !cfg.primary.enabled.value && !cfg.secondary.enabled.value && !cfg.overlay.enabled.value ) ) return false;

		if ( !owner_entity || owner_entity < 0x10000 )
		{
			return false;
		}

		systems::materials::update_outline_glow( settings::g_esp.m_outline_glow );

		const auto owner_handle = memory::safe_read<std::uint32_t>( owner_entity + SCHEMA( "C_BaseEntity", "m_hOwnerEntity"_hash ) ).value_or( 0 );
		if ( owner_handle && owner_handle != 0xffffffff )
		{
			return false;
		}

		const auto game_scene_node = memory::safe_read<std::uintptr_t>( owner_entity + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) ).value_or( 0 );
		if ( game_scene_node && game_scene_node >= 0x10000 )
		{
			const auto parent_node = memory::safe_read<std::uintptr_t>( game_scene_node + SCHEMA( "CGameSceneNode", "m_pParent"_hash ) ).value_or( 0 );
			if ( parent_node )
			{
				return false;
			}
		}

		{
			const auto flags = memory::safe_read<std::uint8_t>( scene_object + 0x78 );
			if ( flags ) {
				(void) memory::safe_write<std::uint8_t>(
					scene_object + 0x78,
					static_cast<std::uint8_t>( *flags & ~( 1u << 3 ) ) );
			}
		}

		const bool primary_is_outline = cfg.primary.enabled.value && settings::esp::is_outline_material( cfg.primary.material.value );
		const bool primary_suppress_original_fill = primary_is_outline && !cfg.primary.filled.value;
		const bool overlay_is_outline = cfg.overlay.enabled.value && settings::esp::is_outline_material( cfg.overlay.material.value );
		const bool overlay_suppress_original_fill = overlay_is_outline && !cfg.overlay.filled.value;
		const bool has_filled_outline =
			( primary_is_outline && cfg.primary.filled.value ) ||
			( overlay_is_outline && cfg.overlay.filled.value );
		const bool suppress_original_fill =
			( primary_suppress_original_fill || overlay_suppress_original_fill ) && !has_filled_outline;
		const bool primary_is_overlay = cfg.primary.enabled.value && settings::esp::is_overlay_material( cfg.primary.material.value );
		const bool secondary_is_overlay = cfg.secondary.enabled.value && settings::esp::is_overlay_material( cfg.secondary.material.value );

		const auto apply_item_layer = [ & ]( const settings::esp::chams_layer& layer )
		{
			if ( settings::esp::is_overlay_material( layer.material.value ) )
				this->apply_overlay( primitive_buffer, original_fn, a1, scene_object, scene_view,
					layer.color, layer.material, &layer.glow );
			else
				this->apply_layer( primitive_buffer, original_fn, a1, scene_object, scene_view,
					layer.color, layer.material, &layer.glow );
		};

		if ( cfg.secondary.enabled.value && secondary_is_overlay )
			apply_item_layer( cfg.secondary );

		const bool has_primary_fill = cfg.primary.enabled.value && !primary_is_overlay;
		if ( has_primary_fill )
			apply_item_layer( cfg.primary );

		const bool has_secondary_fill = cfg.secondary.enabled.value && !secondary_is_overlay;
		if ( has_secondary_fill )
			apply_item_layer( cfg.secondary );

		const bool needs_original_base = !has_primary_fill && !has_secondary_fill &&
			( cfg.overlay.enabled.value || ( primary_is_overlay && !primary_suppress_original_fill ) );
		if ( needs_original_base && !suppress_original_fill )
			original_fn( a1, scene_object, scene_view, primitive_buffer );

		if ( cfg.primary.enabled.value && primary_is_overlay )
			apply_item_layer( cfg.primary );

		if ( cfg.overlay.enabled.value )
			apply_item_layer( cfg.overlay );

		return true;
	}

	void chams::apply_layer( std::uintptr_t primitive_buffer, std::uintptr_t( __fastcall* original_fn )( std::uintptr_t, std::uintptr_t, std::uintptr_t, std::uintptr_t ), std::uintptr_t a1, std::uintptr_t scene_object, std::uintptr_t scene_view, const xdraw::color& color, settings::esp::cham_ids material_id, const settings::esp::outline_glow_config* glow_cfg )
	{
		std::uintptr_t material = 0;
		if ( glow_cfg && ( material_id == settings::esp::cham_ids::outline_glow || material_id == settings::esp::cham_ids::outline_glow_ignorez ) )
		{
			const bool is_iz = ( material_id == settings::esp::cham_ids::outline_glow_ignorez );
			material = systems::materials::get_outline_glow( *glow_cfg, is_iz );
		}
		if ( !material )
			material = systems::materials::find( material_id );
		if ( !material )
			material = systems::materials::find( settings::esp::is_overlay_material( material_id )
				? ( is_ignorez_material( material_id ) ? settings::esp::cham_ids::outlines_ignorez : settings::esp::cham_ids::outlines )
				: ( is_ignorez_material( material_id ) ? settings::esp::cham_ids::matte_ignorez : settings::esp::cham_ids::matte ) );
		if ( !material )
			material = systems::materials::find( is_ignorez_material( material_id )
				? settings::esp::cham_ids::matte_ignorez : settings::esp::cham_ids::matte );
		if ( !material )
			material = systems::materials::find( is_ignorez_material( material_id )
				? settings::esp::cham_ids::flat_ignorez : settings::esp::cham_ids::flat );
		if ( !material )
			return;

		const auto before = detail::read_primitive_buffer( primitive_buffer );
		const auto prev_count = before ? before->count() : -1;

		original_fn( a1, scene_object, scene_view, primitive_buffer );

		const auto after = detail::read_primitive_buffer( primitive_buffer );
		const auto new_count = after ? after->count() : -1;
		if ( !after || prev_count < 0 || prev_count >= new_count )
			return;

		auto draw_color = color;
		apply_outline_glow_pulse( draw_color, material_id, glow_cfg );
		if ( is_glow_material( material_id ) && !is_outline_glow_material( material_id ) )
			keep_glow_color_visible( draw_color );
		const auto prim_color = static_cast<std::uint32_t>( draw_color );
		__try
		{
			for ( auto i = prev_count; i < new_count; ++i )
			{
				const auto prim = after->at_fast( i );
				if ( prim )
				{
					detail::replace_primitive_fast( prim, material, prim_color );
				}
			}
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
		}
	}

	void chams::apply_overlay( std::uintptr_t primitive_buffer, std::uintptr_t( __fastcall* original_fn )( std::uintptr_t, std::uintptr_t, std::uintptr_t, std::uintptr_t ), std::uintptr_t a1, std::uintptr_t scene_object, std::uintptr_t scene_view, const xdraw::color& color, settings::esp::cham_ids material_id, const settings::esp::outline_glow_config* glow_cfg )
	{
		std::uintptr_t material = 0;
		if ( glow_cfg && ( material_id == settings::esp::cham_ids::outline_glow || material_id == settings::esp::cham_ids::outline_glow_ignorez ) )
			material = systems::materials::get_outline_glow( *glow_cfg,
				material_id == settings::esp::cham_ids::outline_glow_ignorez );
		if ( !material )
			material = systems::materials::find( material_id );
		if ( !material )
			material = systems::materials::find( settings::esp::is_overlay_material( material_id )
				? ( is_ignorez_material( material_id ) ? settings::esp::cham_ids::outlines_ignorez : settings::esp::cham_ids::outlines )
				: ( is_ignorez_material( material_id ) ? settings::esp::cham_ids::matte_ignorez : settings::esp::cham_ids::matte ) );
		if ( !material )
			material = systems::materials::find( is_ignorez_material( material_id )
				? settings::esp::cham_ids::matte_ignorez : settings::esp::cham_ids::matte );
		if ( !material )
			material = systems::materials::find( is_ignorez_material( material_id )
				? settings::esp::cham_ids::flat_ignorez : settings::esp::cham_ids::flat );
		if ( !material )
			return;

		const auto before = detail::read_primitive_buffer( primitive_buffer );
		const auto prev_count = before ? before->count() : -1;
		original_fn( a1, scene_object, scene_view, primitive_buffer );

		const auto after = detail::read_primitive_buffer( primitive_buffer );
		const auto new_count = after ? after->count() : -1;
		if ( !after || prev_count < 0 || prev_count >= new_count )
			return;

		auto draw_color = color;
		apply_outline_glow_pulse( draw_color, material_id, glow_cfg );
		if ( is_glow_material( material_id ) && !is_outline_glow_material( material_id ) )
			keep_glow_color_visible( draw_color );
		const auto color_value = static_cast<std::uint32_t>( draw_color );
		__try
		{
			for ( auto i = prev_count; i < new_count; ++i )
			{
				const auto primitive = after->at_fast( i );
				if ( primitive )
				{
					detail::replace_primitive_fast( primitive, material, color_value );
					detail::mark_primitive_last_fast( primitive );
				}
			}
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
		}
	}

	std::uint32_t chams::get_item_group( std::uint32_t schema_hash )
	{
		switch ( schema_hash )
		{
		case "C_DEagle"_hash:
		case "C_WeaponElite"_hash:
		case "C_WeaponFiveSeven"_hash:
		case "C_WeaponGlock"_hash:
		case "C_WeaponHKP2000"_hash:
		case "C_WeaponUSPSilencer"_hash:
		case "C_WeaponP250"_hash:
		case "C_WeaponCZ75a"_hash:
		case "C_WeaponTec9"_hash:
		case "C_WeaponRevolver"_hash:
			return 0;

		case "C_WeaponMAC10"_hash:
		case "C_WeaponMP5SD"_hash:
		case "C_WeaponMP7"_hash:
		case "C_WeaponMP9"_hash:
		case "C_WeaponBizon"_hash:
		case "C_WeaponP90"_hash:
		case "C_WeaponUMP45"_hash:
			return 1;

		case "C_AK47"_hash:
		case "C_WeaponM4A1"_hash:
		case "C_WeaponM4A1Silencer"_hash:
		case "C_WeaponAug"_hash:
		case "C_WeaponFamas"_hash:
		case "C_WeaponGalilAR"_hash:
		case "C_WeaponSG556"_hash:
			return 2;

		case "C_WeaponNOVA"_hash:
		case "C_WeaponSawedoff"_hash:
		case "C_WeaponXM1014"_hash:
		case "C_WeaponMag7"_hash:
			return 3;

		case "C_WeaponAWP"_hash:
		case "C_WeaponG3SG1"_hash:
		case "C_WeaponSCAR20"_hash:
		case "C_WeaponSSG08"_hash:
		case "C_WeaponM249"_hash:
		case "C_WeaponNegev"_hash:
			return 4;

		case "C_HEGrenade"_hash:
		case "C_Flashbang"_hash:
		case "C_SmokeGrenade"_hash:
		case "C_MolotovGrenade"_hash:
		case "C_IncendiaryGrenade"_hash:
		case "C_DecoyGrenade"_hash:
		case "C_C4"_hash:
		case "C_WeaponTaser"_hash:
		case "C_Item_Healthshot"_hash:
		case "C_Knife"_hash:
			return 5;

		default:
			return UINT32_MAX;
		}
	}

} // namespace features::esp::item
