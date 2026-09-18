#pragma once
#include <core/settings.hpp>
#include <external/xdraw/xui/xui.hpp>

namespace rendering::detail {

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
		const bool is_outline = settings::esp::is_outline_material( layer.material.value );
		const bool is_glow_outline = ( layer.material.value == settings::esp::cham_ids::outline_glow ||
									   layer.material.value == settings::esp::cham_ids::outline_glow_ignorez );

		const auto prev_mat = layer.material.value;
		if ( is_through_wall )
		{
			int mat_idx = settings::esp::get_iz_index( layer.material.value );
			if ( xui::combo( "material", mat_idx, settings::esp::k_iz_material_names, settings::esp::k_iz_material_count ) )
			{
				layer.material.value = settings::esp::k_iz_materials[ mat_idx ];
				if ( ( layer.material.value == settings::esp::cham_ids::outline_glow || layer.material.value == settings::esp::cham_ids::outline_glow_ignorez ) &&
					 ( prev_mat != settings::esp::cham_ids::outline_glow && prev_mat != settings::esp::cham_ids::outline_glow_ignorez ) )
				{
					layer.filled.value = true;
				}
			}
		}
		else
		{
			int mat_idx = settings::esp::get_non_iz_index( layer.material.value );
			if ( xui::combo( "material", mat_idx, settings::esp::k_non_iz_material_names, settings::esp::k_non_iz_material_count ) )
			{
				layer.material.value = settings::esp::k_non_iz_materials[ mat_idx ];
				if ( ( layer.material.value == settings::esp::cham_ids::outline_glow || layer.material.value == settings::esp::cham_ids::outline_glow_ignorez ) &&
					 ( prev_mat != settings::esp::cham_ids::outline_glow && prev_mat != settings::esp::cham_ids::outline_glow_ignorez ) )
				{
					layer.filled.value = true;
				}
			}
		}

		xui::color_picker( "color", layer.color, 0.0f, true, is_outline ? &layer.filled.value : nullptr );
		if ( is_outline )
		{
			xui::checkbox( "filled", layer.filled );
		}
		if ( is_glow_outline )
		{
			xui::layout::spacing( 5.0f );
			xui::layout::separator( );
			draw_outline_glow_sliders( popup_id, layer.glow );
		}
		xui::end_popup( );
	}
}

inline static void draw_chams_config( const char* label, const char* id_suffix, settings::esp::chams_config& cfg, bool show_overlay = true, bool show_through_wall = true )
{
	xui::toggle( label, cfg.enabled );

	char label_buf[ 64 ]{};
	char popup_id[ 64 ]{};

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
