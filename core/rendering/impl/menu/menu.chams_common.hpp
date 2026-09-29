#pragma once
#include <core/settings.hpp>
#include <core/systems/systems.hpp>
#include <external/xdraw/xui/xui.hpp>

namespace rendering::detail {

struct chams_material_options
{
	std::array<const char*, settings::esp::k_non_iz_material_count> names{};
	std::array<settings::esp::cham_ids, settings::esp::k_non_iz_material_count> ids{};
	int count{};
};

inline static chams_material_options get_chams_material_options( bool through_wall )
{
	chams_material_options options{};
	const auto append = [ &options ]( const char* name, settings::esp::cham_ids id )
	{
		if ( systems::materials::find( id ) )
		{
			options.names[ options.count ] = name;
			options.ids[ options.count ] = id;
			++options.count;
		}
	};

	if ( through_wall )
	{
		for ( int i = 0; i < settings::esp::k_iz_material_count; ++i )
			append( settings::esp::k_iz_material_names[ i ], settings::esp::k_iz_materials[ i ] );
	}
	else
	{
		for ( int i = 0; i < settings::esp::k_non_iz_material_count; ++i )
			append( settings::esp::k_non_iz_material_names[ i ], settings::esp::k_non_iz_materials[ i ] );
	}

	return options;
}

inline static bool draw_chams_material_combo( const char* label, config::enm<settings::esp::cham_ids>& material, bool through_wall )
{
	const auto options = get_chams_material_options( through_wall );
	if ( options.count == 0 )
		return false;

	int selected = -1;
	for ( int i = 0; i < options.count; ++i )
	{
		if ( options.ids[ i ] == material.value )
		{
			selected = i;
			break;
		}
	}

	if ( selected < 0 )
	{
		const auto preferred = through_wall ? settings::esp::cham_ids::matte_ignorez : settings::esp::cham_ids::matte;
		for ( int i = 0; i < options.count; ++i )
		{
			if ( options.ids[ i ] == preferred )
			{
				selected = i;
				break;
			}
		}
		if ( selected < 0 )
			selected = 0;
		material.value = options.ids[ selected ];
	}

	if ( !xui::combo( label, selected, options.names.data(), options.count ) )
		return false;

	material.value = options.ids[ selected ];
	return true;
}

inline static void reset_filled_for_new_outline( settings::esp::chams_layer& layer, settings::esp::cham_ids previous_material )
{
	if ( !settings::esp::is_outline_material( previous_material ) &&
		 settings::esp::is_outline_material( layer.material.value ) )
	{
		layer.filled.value = false;
	}
}

inline static void draw_outline_glow_sliders( const char* id_suffix, settings::esp::outline_glow_config& cfg )
{
	char buf[ 64 ]{};

	xui::text( "glow settings", tokens::col_accent );

	std::snprintf( buf, sizeof( buf ), "intensity##%s", id_suffix );
	xui::slider_float( buf, cfg.intensity, 1.0f, 50.0f, "%.1f" );

	std::snprintf( buf, sizeof( buf ), "thickness##%s", id_suffix );
	xui::slider_float( buf, cfg.thickness, 0.5f, 10.0f, "%.1f" );

	std::snprintf( buf, sizeof( buf ), "softness##%s", id_suffix );
	xui::slider_float( buf, cfg.softness, 0.2f, 4.0f, "%.2f" );

	std::snprintf( buf, sizeof( buf ), "opacity##%s", id_suffix );
	xui::slider_float( buf, cfg.opacity, 0.0f, 1.0f, "%.2f" );

	std::snprintf( buf, sizeof( buf ), "inner spread##%s", id_suffix );
	xui::slider_float( buf, cfg.inner_spread, 0.0f, 1.0f, "%.2f" );

	std::snprintf( buf, sizeof( buf ), "pulse speed##%s", id_suffix );
	xui::slider_float( buf, cfg.pulse_speed, 0.0f, 5.0f, "%.1f" );
}

inline static void draw_chams_layer( const char* label, const char* popup_id, settings::esp::chams_layer& layer, bool is_through_wall = false )
{
	xui::toggle( label, layer.enabled );
	if ( xui::begin_popup( popup_id, 220.0f ) )
	{
		const bool is_glow_outline = ( layer.material.value == settings::esp::cham_ids::outline_glow ||
									   layer.material.value == settings::esp::cham_ids::outline_glow_ignorez );

		const auto prev_mat = layer.material.value;
		if ( draw_chams_material_combo( "material", layer.material, is_through_wall ) )
			reset_filled_for_new_outline( layer, prev_mat );

		xui::color_picker( "color", layer.color );
		if ( settings::esp::is_outline_material( layer.material.value ) )
			xui::toggle( "filled", layer.filled );
		if ( is_glow_outline )
		{
			xui::layout::spacing( 5.0f );
			xui::layout::separator( );
			draw_outline_glow_sliders( popup_id, layer.glow );
		}
		xui::end_popup( );
	}
}

inline static void draw_chams_config( const char* label, const char* id_suffix, settings::esp::chams_config& cfg, bool show_overlay = true, bool show_through_wall = true, float* duration = nullptr )
{
	xui::toggle( label, cfg.enabled );

	char label_buf[ 64 ]{};
	char popup_id[ 64 ]{};
	if (duration) {
		std::snprintf(popup_id, sizeof(popup_id), "##chams_settings_%s", id_suffix);
		if (xui::begin_popup(popup_id, 220.0f)) {
			xui::slider_float("duration##ft", *duration, 0.05f, 5.0f, "%.2f s");
			xui::end_popup();
		}
	}

	std::snprintf( label_buf, sizeof( label_buf ), "primary layer##%s", id_suffix );
	std::snprintf( popup_id, sizeof( popup_id ), "##primary_%s", id_suffix );
	draw_chams_layer( label_buf, popup_id, cfg.primary, false );

	if ( show_through_wall )
	{
		std::snprintf( label_buf, sizeof( label_buf ), "through wall##%s", id_suffix );
		std::snprintf( popup_id, sizeof( popup_id ), "##secondary_%s", id_suffix );
		draw_chams_layer( label_buf, popup_id, cfg.secondary, true );
	}

	if ( show_overlay )
	{
		std::snprintf( label_buf, sizeof( label_buf ), "overlay##%s", id_suffix );
		std::snprintf( popup_id, sizeof( popup_id ), "##overlay_%s", id_suffix );
		draw_chams_layer( label_buf, popup_id, cfg.overlay, false );
	}
}

} // namespace rendering::detail
