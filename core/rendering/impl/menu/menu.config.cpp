#include <pch/pch.hpp>
#include <utilities/math/math.hpp>
#include <core/settings.hpp>
#include <core/features/features.hpp>
#include <core/hooks/hooks.hpp>
#include <utilities/logging/logging.hpp>

#include "../../rendering.hpp"

namespace rendering {

	namespace detail {

		std::string name_buf{};
		std::vector<std::wstring> config_list{};
		auto selected{ -1 };
		auto needs_refresh{ true };
		auto confirm_save{ false };
		auto confirm_delete{ false };
		auto confirm_reset{ false };
		auto confirm_timer{ 0.0f };
		auto confirm_reset_timer{ 0.0f };

		auto reset_popup_open{ false };
		auto reset_target_idx{ -1 };
		std::string reset_target_name{};
		float reset_popup_x{ 0.0f };
		float reset_popup_y{ 0.0f };

		// Anchor of the top-bar config selector; the profile manager hangs below it.
		float cfg_anchor_x{ 0.0f };
		float cfg_anchor_y{ 0.0f };
		float cfg_anchor_w{ 0.0f };
		float cfg_anchor_h{ 0.0f };

		static inline void wide_to_utf8( const std::wstring& wide, char* out, int out_size )
		{
			WideCharToMultiByte( CP_UTF8, 0, wide.c_str( ), -1, out, out_size, nullptr, nullptr );
		}

		static inline std::wstring utf8_to_wide( const std::string& utf8 )
		{
			wchar_t buf[ 128 ]{};
			MultiByteToWideChar( CP_UTF8, 0, utf8.c_str( ), -1, buf, 128 );
			return buf;
		}

		static inline bool config_matches_search( const std::wstring& wname )
		{
			if ( detail::name_buf.empty( ) )
			{
				return true;
			}

			const auto wsearch = utf8_to_wide( detail::name_buf );

			// If a config is selected and name_buf is its exact name, keep all configs visible
			if ( detail::selected >= 0 && detail::selected < static_cast< int >( detail::config_list.size( ) ) )
			{
				if ( _wcsicmp( detail::config_list[ detail::selected ].c_str( ), wsearch.c_str( ) ) == 0 )
				{
					return true;
				}
			}

			std::wstring lower_name = wname;
			std::wstring lower_search = wsearch;

			for ( auto& wc : lower_name )
			{
				wc = static_cast< wchar_t >( towlower( wc ) );
			}

			for ( auto& wc : lower_search )
			{
				wc = static_cast< wchar_t >( towlower( wc ) );
			}

			return lower_name.find( lower_search ) != std::wstring::npos;
		}

		static inline std::string selected_name( )
		{
			if ( detail::selected < 0 || detail::selected >= static_cast< int >( detail::config_list.size( ) ) )
			{
				return {};
			}

			char narrow[ 128 ]{};
			wide_to_utf8( detail::config_list[ detail::selected ], narrow, sizeof( narrow ) );
			return narrow;
		}

		static inline bool config_exists( const std::string& name )
		{
			if ( name.empty( ) )
			{
				return false;
			}
			const auto wname = utf8_to_wide( name );
			for ( const auto& cfg : detail::config_list )
			{
				if ( _wcsicmp( cfg.c_str( ), wname.c_str( ) ) == 0 )
				{
					return true;
				}
			}
			return false;
		}

		static inline void reset_defaults( )
		{
            xui::slider_binds::reset();
			config::reset_bind_assignments();
			auto& reg = config::detail::get_registry( );

			for ( auto& f : reg.fields )
			{
				char key_str[ 12 ];
				std::snprintf( key_str, sizeof( key_str ), "%08x", f.key );

				auto def = reg.defaults.find( key_str );
				if ( def != reg.defaults.end( ) )
				{
					config::serial::json_to_field( *def, f );
				}
			}

			xui::binds::reset_runtime();

			// Preserve captured originals so removed cosmetics can be restored on the game thread.
			features::changer::g_guns.invalidate( );
			features::changer::g_knives.invalidate( );
			features::changer::g_skin_sync.on_sync_toggled( );
			settings::finalize_binds( );
			settings::g_world.update_active( rendering::g_widgets.s_map_name );
			rendering::g_menu.apply_theme_preset( settings::g_misc.menu_palette.value );
			xui::tooltips::set_enabled( settings::g_misc.tooltips.value );
		}

	} // namespace detail

	void menu::draw_config_shortcut( float x, float y, float w, float h, bool interactive )
	{
		auto& dl = xui::draw::current( );
		auto& ctx = xui::ctx( );
		auto& input = ctx.input;
		const auto dt = xdraw::delta_time( );

		detail::cfg_anchor_x = x;
		detail::cfg_anchor_y = y;
		detail::cfg_anchor_w = w;
		detail::cfg_anchor_h = h;

		// Keep the top-bar list cached; refresh it periodically and immediately after edits.
		static std::vector<std::wstring> names;
		static float refresh_in{};
		static float save_flash{};
		refresh_in -= dt;
		if ( refresh_in <= 0.0f )
		{
			names = config::registry::list( );
			refresh_in = 1.0f;
		}
		save_flash = std::max( 0.0f, save_flash - dt * 3.0f );

		constexpr float rounding{ 6.0f };
		const float button_size = std::max( 22.0f, h - 8.0f );
		const xui::rect save_rect{ x + 4.0f, y + ( h - button_size ) * 0.5f, button_size, button_size };
		const xui::rect new_rect{ x + w - 4.0f - button_size, save_rect.y, button_size, button_size };
		const float picker_x = save_rect.right( ) + 4.0f;
		const float picker_w = std::max( 70.0f, new_rect.x - picker_x - 4.0f );

		// Card + glass background for the whole group.
		dl.rect_filled_blurred( x, y, w, h, xdraw::corner_radius{ tokens::card_rounding }, xdraw::color{ 45, 50, 60, 150 } );
		dl.rect_filled( x, y, w, h, tokens::col_card.alpha( 220 ), xdraw::corner_radius{ tokens::card_rounding } );
		dl.rect( x, y, w, h, tokens::col_border.alpha( 180 ), xdraw::corner_radius{ tokens::card_rounding }, 1.0f );

		const auto save_hovered = interactive && input.in_rect( save_rect ) && !ctx.overlay_blocking( );
		const auto save_anim = xui::anim::lerp( xui::fnv1a( "menu_quick_config_save" ), save_hovered ? 1.0f : 0.0f, 14.0f );
		if ( save_anim > 0.01f || save_flash > 0.0f )
		{
			dl.rect_filled( save_rect.x, save_rect.y, save_rect.w, save_rect.h,
				tokens::col_accent.alpha( static_cast< std::uint8_t >( 45.0f * std::max( save_anim, save_flash ) ) ),
				xdraw::corner_radius{ rounding } );
		}
		const auto glyph_col = tokens::col_accent.alpha( static_cast< std::uint8_t >( 90.0f + 165.0f * std::max( save_anim, save_flash ) ) );
		const auto cx = save_rect.x + save_rect.w * 0.5f;
		const auto cy = save_rect.y + save_rect.h * 0.5f;
		dl.rect( cx - 4.5f, cy - 4.5f, 9.0f, 9.0f, glyph_col, xdraw::corner_radius{ 1.5f }, 1.2f );
		dl.rect_filled( cx - 2.0f, cy - 4.5f, 4.0f, 2.6f, glyph_col );
		dl.rect( cx - 2.8f, cy - 0.5f, 5.6f, 5.0f, glyph_col, xdraw::corner_radius{ 0.8f }, 1.0f );
		if ( save_hovered && input.mouse_clicked )
		{
			input.mouse_clicked = false;
			if ( config::registry::save_active( ) )
			{
				save_flash = 0.6f;
				names = config::registry::list( );
				refresh_in = 1.0f;
			}
		}

		const auto plus_hovered = interactive && input.in_rect( new_rect ) && !ctx.overlay_blocking( );
		const auto plus_anim = xui::anim::lerp( xui::fnv1a( "menu_quick_config_new" ), plus_hovered ? 1.0f : 0.0f, 14.0f );
		if ( plus_hovered || plus_anim > 0.01f )
		{
			dl.rect_filled( new_rect.x, new_rect.y, new_rect.w, new_rect.h,
				tokens::col_accent.alpha( static_cast< std::uint8_t >( 38.0f * plus_anim ) ),
				xdraw::corner_radius{ rounding } );
		}
		dl.line( new_rect.x + new_rect.w * 0.5f - 4.0f, cy, new_rect.x + new_rect.w * 0.5f + 4.0f, cy,
			tokens::col_text.alpha( static_cast< std::uint8_t >( 150.0f + 105.0f * plus_anim ) ), 1.4f );
		dl.line( new_rect.x + new_rect.w * 0.5f, cy - 4.0f, new_rect.x + new_rect.w * 0.5f, cy + 4.0f,
			tokens::col_text.alpha( static_cast< std::uint8_t >( 150.0f + 105.0f * plus_anim ) ), 1.4f );
		if ( plus_hovered && input.mouse_clicked )
		{
			input.mouse_clicked = false;
			if ( names.empty( ) ) names = config::registry::list( );
			std::wstring fresh = L"config";
			for ( int suffix = 2; std::find( names.begin( ), names.end( ), fresh ) != names.end( ); ++suffix )
				fresh = L"config " + std::to_wstring( suffix );
			if ( config::registry::save( fresh ) )
			{
				names = config::registry::list( );
				refresh_in = 1.0f;
				detail::needs_refresh = true;
			}
		}

		if ( names.empty( ) ) names.push_back( L"default" );
		std::vector<std::string> labels;
		labels.reserve( names.size( ) );
		for ( const auto& name : names )
		{
			char buffer[ 128 ]{};
			detail::wide_to_utf8( name, buffer, static_cast< int >( sizeof( buffer ) ) );
			labels.emplace_back( buffer );
		}

		int active = 0;
		for ( std::size_t i = 0; i < names.size( ); ++i )
		{
			if ( names[ i ] == config::registry::g_active_config )
			{
				active = static_cast< int >( i );
				break;
			}
		}

		if ( active < 0 || active >= static_cast< int >( labels.size( ) ) )
		{
			active = 0;
		}

		// Single config list: the selector itself opens the profile manager dropdown.
		const xui::rect picker_rect{ picker_x, save_rect.y, picker_w, button_size };
		const auto picker_hovered = interactive && input.in_rect( picker_rect ) && !ctx.overlay_blocking( );
		const auto picker_anim = xui::anim::lerp( xui::fnv1a( "menu_config_picker" ), picker_hovered ? 1.0f : 0.0f, 14.0f );
		const auto picker_active = this->m_config_popup_open ? 1.0f : 0.0f;

		// No resting background: the selected profile name sits directly on the card, and
		// only hover / open draws a soft pill around it.
		const auto picker_base = std::max( picker_anim, picker_active );
		if ( picker_base > 0.01f )
		{
			dl.rect_filled( picker_rect.x, picker_rect.y, picker_rect.w, picker_rect.h,
				tokens::col_elevated.alpha( static_cast< std::uint8_t >( 150.0f * picker_base ) ),
				xdraw::corner_radius{ tokens::btn_rounding } );
			dl.rect( picker_rect.x, picker_rect.y, picker_rect.w, picker_rect.h,
				tokens::col_accent.alpha( static_cast< std::uint8_t >( 170.0f * picker_base ) ),
				xdraw::corner_radius{ tokens::btn_rounding }, 1.0f );
		}

		xdraw::push_font( g_fonts.inter_medium[ fonts::size::petite ] );
		const auto picker_label = xui::truncate( labels[ active ], picker_rect.w - 24.0f );
		const auto picker_text_h = xdraw::measure_text( picker_label ).second;
		dl.text( picker_rect.x + 9.0f, picker_rect.y + ( picker_rect.h - picker_text_h ) * 0.5f, picker_label,
			xui::lerp( tokens::col_text_dim, tokens::col_text, picker_base ) );
		xdraw::pop_font( );

		const auto chev_x = picker_rect.x + picker_rect.w - 9.0f;
		const auto chev_y = picker_rect.y + picker_rect.h * 0.5f;
		const auto chev_col = xui::lerp( tokens::col_text_dim, tokens::col_accent, picker_base );
		dl.line( chev_x - 4.0f, chev_y - 2.0f, chev_x, chev_y + 2.0f, chev_col, 1.4f );
		dl.line( chev_x, chev_y + 2.0f, chev_x + 4.0f, chev_y - 2.0f, chev_col, 1.4f );

		if ( picker_hovered && input.mouse_clicked )
		{
			input.mouse_clicked = false;
			this->m_config_popup_open = !this->m_config_popup_open;
			detail::needs_refresh = true;
			detail::reset_popup_open = false;
		}
	}

	// Full profile manager (list, search, save / delete / refresh, right-click reset)
	// hosted in a dropdown panel under the top-bar selector.
	void menu::draw_config_dropdown( )
	{
		const auto anim = xui::anim::lerp( xui::fnv1a( "menu_config_popup_anim" ), this->m_config_popup_open ? 1.0f : 0.0f, 16.0f );
		if ( anim < 0.005f )
		{
			return;
		}

		if ( detail::needs_refresh )
		{
			detail::config_list = config::registry::list( );
			detail::needs_refresh = false;

			if ( detail::selected >= static_cast< int >( detail::config_list.size( ) ) )
			{
				detail::selected = -1;
			}
		}

		const auto dt = xdraw::delta_time( );

		if ( detail::confirm_save || detail::confirm_delete )
		{
			detail::confirm_timer += dt;

			if ( detail::confirm_timer > 3.5f )
			{
				detail::confirm_save = false;
				detail::confirm_delete = false;
			}
		}

		if ( detail::confirm_reset )
		{
			detail::confirm_reset_timer += dt;

			if ( detail::confirm_reset_timer > 3.5f )
			{
				detail::confirm_reset = false;
			}
		}

		auto& ctx = xui::ctx( );
		auto& input = ctx.input;

		constexpr float k_panel_w{ 268.0f };

		// The list (and so the whole panel) is sized to the number of matching profiles
		// so the manager is never taller than its content. The height is smoothed so
		// filtering with the search field does not snap.
		constexpr float k_row_h{ 28.0f };
		int list_rows{ 0 };
		for ( const auto& wname : detail::config_list )
		{
			if ( detail::config_matches_search( wname ) )
			{
				++list_rows;
			}
		}
		list_rows = std::max( 1, list_rows );

		const auto list_target_h = std::clamp(
			( k_row_h + ctx.style.item_spacing_y ) * static_cast< float >( list_rows ) - ctx.style.item_spacing_y + 16.0f,
			k_row_h + 16.0f, 190.0f );
		const auto list_h = xui::anim::lerp( xui::fnv1a( "menu_cfg_list_h" ), list_target_h, 12.0f );

		// Header text + search field + two gaps + list + button row + bottom padding.
		const auto panel_h = list_h + 114.0f;
		const auto anchor_x = detail::cfg_anchor_x;
		const auto anchor_y = detail::cfg_anchor_y;
		const auto anchor_w = detail::cfg_anchor_w;
		const auto anchor_h = detail::cfg_anchor_h;
		const auto panel_x = std::clamp( anchor_x + anchor_w - k_panel_w, this->m_x + 14.0f, std::max( this->m_x + 14.0f, this->m_x + this->m_w - k_panel_w - 14.0f ) );
		const auto panel_target_y = anchor_y + anchor_h + 8.0f;
		const auto panel_y = panel_target_y + ( 1.0f - anim ) * 8.0f;
		const xui::rect panel{ panel_x, panel_y, k_panel_w, panel_h };
		const xui::rect anchor_rect{ anchor_x, anchor_y, anchor_w, anchor_h };

		const auto mouse_in_panel = input.in_rect( panel );
		const auto mouse_in_anchor = input.in_rect( anchor_rect );
		const auto overlays_open = xui::overlays::blocks_background( );

		if ( this->m_config_popup_open && input.mouse_clicked && !overlays_open && ( mouse_in_anchor || !mouse_in_panel ) )
		{
			// Clicking the selector again, or anywhere outside, closes the manager.
			this->m_config_popup_open = false;
			detail::reset_popup_open = false;
			detail::confirm_reset = false;
			input.mouse_clicked = false;
		}

		for ( const auto vk : input.key_presses( ) )
		{
			if ( vk == VK_ESCAPE && this->m_config_popup_open )
			{
				this->m_config_popup_open = false;
				detail::reset_popup_open = false;
				detail::confirm_reset = false;
				break;
			}
		}

		// Swallow input while the panel fades out so nothing behind reacts.
		// (The ambience modal outranks this panel and keeps its own input.)
		const auto ambience_active = this->m_ambience_open || this->m_ambience_anim > 0.001f;
		if ( !this->m_config_popup_open && !ambience_active )
		{
			input.mouse_clicked = false;
			input.mouse_down = false;
		}

		auto& top_dl = xdraw::get( xdraw::layer::top );
		const auto alpha_mult = std::clamp( anim, 0.0f, 1.0f );

		// Everything the manager emits (background, widgets and the reset popup) is
		// faded as one unit at the end so the panel never fades before its widgets.
		const auto vtx_start = top_dl.vertices.size( );

		// Soft ambient drop shadow around the panel
		top_dl.rect_filled( panel_x - 10.0f, panel_y - 6.0f, k_panel_w + 20.0f, panel_h + 14.0f,
			xdraw::color{ 0, 0, 0, static_cast< std::uint8_t >( 22.0f * alpha_mult ) }, xdraw::corner_radius{ 16.0f } );
		top_dl.rect_filled( panel_x - 5.0f, panel_y - 3.0f, k_panel_w + 10.0f, panel_h + 8.0f,
			xdraw::color{ 0, 0, 0, static_cast< std::uint8_t >( 40.0f * alpha_mult ) }, xdraw::corner_radius{ 13.0f } );
		top_dl.rect_filled( panel_x, panel_y + 2.0f, k_panel_w, panel_h,
			xdraw::color{ 0, 0, 0, static_cast< std::uint8_t >( 65.0f * alpha_mult ) }, xdraw::corner_radius{ 10.0f } );

		top_dl.rect_filled_blurred( panel_x, panel_y, k_panel_w, panel_h, xdraw::corner_radius{ 10.0f },
			xdraw::color{ 255, 255, 255, static_cast< std::uint8_t >( 255.0f * alpha_mult ) } );
		const auto glass_top = xdraw::color{ 255, 255, 255, static_cast< std::uint8_t >( 75.0f * alpha_mult ) };
		const auto glass_bot = xdraw::color{ 240, 245, 255, static_cast< std::uint8_t >( 45.0f * alpha_mult ) };
		top_dl.rect_filled_gradient( panel_x, panel_y, k_panel_w, panel_h,
			glass_top, glass_top, glass_bot, glass_bot, xdraw::corner_radius{ 10.0f } );
		top_dl.rect( panel_x, panel_y, k_panel_w, panel_h,
			xdraw::color{ 255, 255, 255, static_cast< std::uint8_t >( 160.0f * alpha_mult ) }, xdraw::corner_radius{ 10.0f }, 1.0f );
		top_dl.line( panel_x + 12.0f, panel_y + 0.5f, panel_x + k_panel_w - 12.0f, panel_y + 0.5f,
			xdraw::color{ 255, 255, 255, static_cast< std::uint8_t >( 200.0f * alpha_mult ) }, 1.0f );

		xdraw::push_font( g_fonts.inter_bold[ fonts::size::petite ] );
		top_dl.text( panel_x + 14.0f, panel_y + 12.0f, "CONFIG PROFILES",
			tokens::col_text );
		xdraw::pop_font( );

		// The panel owns the mouse while the manager is open, everything else is blocked.
		if ( mouse_in_panel )
		{
			ctx.inside_overlay = xui::fnv1a( "menu_config_popup" );
		}

		// Widgets keep the live layout/window state but draw on the top layer.
		xui::draw::push_layer( xdraw::layer::top );
		auto& dl = xui::draw::current( );

		if ( auto* win = xui::layout::current_window( ) )
		{
			const auto saved_cursor_x = win->cursor_x;
			const auto saved_cursor_y = win->cursor_y;
			const auto saved_line_h = win->line_h;
			const auto saved_bounds_w = win->bounds.w;
			const auto saved_start_x = win->start_x;
			const auto saved_on_same_line = win->on_same_line;

			win->bounds.w = std::max( 40.0f, panel_x - win->bounds.x + k_panel_w );
			xui::layout::set_cursor( panel_x + 14.0f - this->m_x, panel_y + 32.0f - this->m_y );

			xui::text_input( "##cfg_name", detail::name_buf, 64, "config name / search..." );

			const auto avail_w = xui::layout::avail( ).first;
			constexpr auto row_h{ k_row_h };

			// Filled in while the list is open; used for the scroll fades below.
			xui::rect list_rect{};
			auto list_top_fade{ 0.0f };
			auto list_bottom_fade{ 0.0f };

			xui::layout::spacing( 4.0f );

			if ( xui::begin_child( "##cfg_list", avail_w, list_h, true, false ) )
			{
				const auto row_w = xui::layout::avail( ).first;
				const auto child_win = xui::layout::current_window( );
				const auto child_bounds = child_win ? child_win->bounds : xui::rect{};
				auto visible_rows{ 0 };

				for ( auto i = 0; i < static_cast< int >( detail::config_list.size( ) ); ++i )
				{
					const auto& wname = detail::config_list[ i ];

					if ( !detail::config_matches_search( wname ) )
					{
						continue;
					}

					char narrow[ 128 ]{};
					detail::wide_to_utf8( wname, narrow, sizeof( narrow ) );

					const auto row = xui::layout::item( row_w, row_h );
					const auto in_view = ( row.y + row.h >= child_bounds.y && row.y <= child_bounds.y + child_bounds.h );
					const auto is_selected = ( detail::selected == i );
					const auto is_hovered = in_view && input.in_rect( row ) && !ctx.overlay_blocking( );

					if ( is_hovered && input.mouse_clicked )
					{
						if ( config::registry::load( wname ) )
						{
							detail::selected = i;
							detail::name_buf = narrow;
							detail::confirm_save = false;
							detail::confirm_delete = false;
							detail::reset_popup_open = false;
							// Only finalize the new profile after a successful load.
							features::changer::g_guns.invalidate( );
							features::changer::g_knives.invalidate( );
							features::changer::g_skin_sync.on_sync_toggled( );
							settings::finalize_binds( );
							settings::g_world.update_active( rendering::g_widgets.s_map_name );
							rendering::g_menu.apply_theme_preset( settings::g_misc.menu_palette.value );
							xui::tooltips::set_enabled( settings::g_misc.tooltips.value );
							hooks::cheat::trigger_lobby_music( static_cast< std::uint16_t >( settings::g_changer.music.id ) );
						}
						else
						{
							logging::console::print( "[config] failed to load '{}'\n", narrow );
							logging::popup::show( "Config load failed", "Could not load the selected profile. Check the config file and disk access." );
						}

						input.mouse_clicked = false;
					}
					else if ( is_hovered && input.rmb_clicked )
					{
						detail::selected = i;
						detail::name_buf = narrow;
						detail::reset_target_idx = i;
						detail::reset_target_name = narrow;
						detail::reset_popup_open = true;
						detail::confirm_reset = false;
						detail::confirm_reset_timer = 0.0f;
						detail::reset_popup_x = input.mouse_x;
						detail::reset_popup_y = input.mouse_y;
					}

					const auto hover_anim = xui::anim::lerp( xui::fnv1a( "cfgrow" ) + i, is_hovered ? 1.0f : 0.0f, 14.0f );
					const auto sel_anim = xui::anim::lerp( xui::fnv1a( "cfgsel" ) + i, is_selected ? 1.0f : 0.0f, 10.0f );

					if ( sel_anim > 0.01f )
					{
						// Selection is marked by a slim accent bar + accent text instead of a
						// full-row background, which read as a crooked block.
						dl.rect_filled( row.x + 2.0f, row.y + 5.0f, 2.0f, row.h - 10.0f,
							tokens::col_accent.alpha( static_cast< std::uint8_t >( 220.0f * sel_anim ) ),
							xdraw::corner_radius{ 1.0f } );
					}
					else if ( hover_anim > 0.01f )
					{
						dl.rect_filled( row.x, row.y, row.w, row.h, tokens::col_elevated.alpha( static_cast< std::uint8_t >( 255.0f * hover_anim * 0.5f ) ), xdraw::corner_radius{ 6.0f } );
					}

					const auto text_col = is_selected
						? xui::lerp( tokens::col_text, tokens::col_accent, sel_anim )
						: xui::lerp( tokens::col_text_dim, tokens::col_text, hover_anim );

					dl.text( row.x + 9.0f, row.y + ( row_h - xdraw::measure_text( narrow ).second ) * 0.5f, narrow, text_col );

					visible_rows++;
				}

				if ( visible_rows == 0 )
				{
					const auto row = xui::layout::item( row_w, row_h );
					dl.text( row.x + 9.0f, row.y + 6.0f, detail::config_list.empty( ) ? "no configs found" : "no matches", tokens::col_text_dim );
				}

				// The search field above and the buttons below stay put; only this region
				// scrolls, so its clipped ends need to fade out while it can scroll.
				list_rect = child_bounds;
				if ( child_win )
				{
					const auto scrolled = ctx.child_scroll_cache[ child_win->group_id ].scroll;
					const auto true_content_h = child_win->content_h + child_win->scroll_y + ctx.style.window_pad_y;
					const auto max_scroll = std::max( 0.0f, true_content_h - child_bounds.h );
					constexpr auto fade_span{ 24.0f };
					list_top_fade = std::clamp( scrolled / fade_span, 0.0f, 1.0f );
					list_bottom_fade = std::clamp( ( max_scroll - scrolled ) / fade_span, 0.0f, 1.0f );
				}

				xui::end_child( );
			}

			// Soft scrims so rows dissolve into the panel at the clipped ends.
			if ( list_rect.w > 0.0f && ( list_top_fade > 0.01f || list_bottom_fade > 0.01f ) )
			{
				constexpr auto fade_h{ 16.0f };
				const auto scrim_base = tokens::col_card;
				auto draw_scrim = [ & ]( float y, bool from_top, float strength )
				{
					auto solid = scrim_base;
					solid.a = static_cast< std::uint8_t >( 255.0f * strength );
					const auto clear = xdraw::color{ solid.r, solid.g, solid.b, 0 };
					if ( from_top )
					{
						dl.rect_filled_gradient( list_rect.x, y, list_rect.w, fade_h,
							solid, solid, clear, clear, xdraw::corner_radius::top( 6.0f ) );
					}
					else
					{
						dl.rect_filled_gradient( list_rect.x, y, list_rect.w, fade_h,
							clear, clear, solid, solid, xdraw::corner_radius::bottom( 6.0f ) );
					}
				};
				if ( list_top_fade > 0.01f )
				{
					draw_scrim( list_rect.y, true, list_top_fade );
				}
				if ( list_bottom_fade > 0.01f )
				{
					draw_scrim( list_rect.y + list_rect.h - fade_h, false, list_bottom_fade );
				}
			}

			// 3 buttons: save, delete, refresh
			xui::layout::spacing( 4.0f );

			constexpr auto btn_h{ 26.0f };
			const auto btn_w = ( avail_w - xui::ctx( ).style.item_spacing_x * 2.0f ) / 3.0f;
			const auto has_selection = detail::selected >= 0 && detail::selected < static_cast< int >( detail::config_list.size( ) );
			const auto save_name = detail::name_buf.empty( ) ? detail::selected_name( ) : detail::name_buf;
			const auto can_save = !save_name.empty( );
			const auto already_exists = detail::config_exists( save_name );

			// Button 1: Save
			std::string save_label;
			if ( detail::confirm_save )
			{
				save_label = already_exists ? "overwrite?" : "confirm?";
			}
			else
			{
				save_label = "save";
			}

			if ( xui::button( save_label, btn_w, btn_h ) && can_save )
			{
				if ( !detail::confirm_save )
				{
					detail::confirm_save = true;
					detail::confirm_delete = false;
					detail::confirm_timer = 0.0f;
				}
				else
				{
					config::registry::save( detail::utf8_to_wide( save_name ) );
					detail::needs_refresh = true;
					detail::confirm_save = false;
				}
			}

			xui::layout::same_line( );

			// Button 2: Delete
			std::string delete_label;
			if ( detail::confirm_delete )
			{
				delete_label = "confirm?";
			}
			else
			{
				delete_label = "delete";
			}

			if ( xui::button( delete_label, btn_w, btn_h ) && has_selection )
			{
				if ( !detail::confirm_delete )
				{
					detail::confirm_delete = true;
					detail::confirm_save = false;
					detail::confirm_timer = 0.0f;
				}
				else
				{
					config::registry::remove( detail::config_list[ detail::selected ] );
					detail::selected = -1;
					detail::name_buf.clear( );
					detail::needs_refresh = true;
					detail::confirm_delete = false;
				}
			}

			xui::layout::same_line( );

			// Button 3: Refresh
			if ( xui::button( "refresh", btn_w, btn_h ) )
			{
				detail::needs_refresh = true;
				detail::confirm_save = false;
				detail::confirm_delete = false;
				detail::reset_popup_open = false;
			}

			win->bounds.w = saved_bounds_w;
			win->cursor_x = saved_cursor_x;
			win->cursor_y = saved_cursor_y;
			win->line_h = saved_line_h;
			win->start_x = saved_start_x;
			win->on_same_line = saved_on_same_line;
		}

		xui::draw::pop_layer( );
		ctx.inside_overlay = xui::null_id;

		// Reset popup (context menu on right-click of a config row)
		constexpr auto popup_w{ 128.0f };
		constexpr auto popup_h{ 28.0f };
		if ( detail::reset_popup_open && detail::reset_target_idx >= 0
			&& detail::reset_target_idx < static_cast< int >( detail::config_list.size( ) ) )
		{
			const auto px = std::clamp( detail::reset_popup_x, panel_x + 6.0f, panel_x + k_panel_w - popup_w - 6.0f );
			const auto py = std::clamp( detail::reset_popup_y, panel_y + 6.0f, panel_y + panel_h - popup_h - 6.0f );
			const auto popup_rect = xui::rect{ px, py, popup_w, popup_h };

			const auto clicked_any = input.mouse_clicked || input.rmb_clicked;
			const auto is_hovered = input.in_rect( popup_rect );

			if ( clicked_any && !is_hovered )
			{
				detail::reset_popup_open = false;
				detail::confirm_reset = false;
			}
			else
			{
				// Blurred glass
				top_dl.rect_filled_blurred( px, py, popup_w, popup_h, xdraw::corner_radius{ 6.0f }, xdraw::color{ 255, 255, 255, 255 } );
				// Frosted body
				const auto reset_glass_top = xdraw::color{ 255, 255, 255, 75 };
				const auto reset_glass_bot = xdraw::color{ 240, 245, 255, 45 };
				top_dl.rect_filled_gradient( px, py, popup_w, popup_h, reset_glass_top, reset_glass_top, reset_glass_bot, reset_glass_bot, xdraw::corner_radius{ 6.0f } );
				// Top specular line
				top_dl.line( px + 6.0f, py + 0.5f, px + popup_w - 6.0f, py + 0.5f, xdraw::color{ 255, 255, 255, 200 }, 1.0f );
				// Border
				const auto is_danger = is_hovered || detail::confirm_reset;
				const auto border_col = is_danger ? xdraw::color{ 255, 75, 85, 200 } : xdraw::color{ 255, 255, 255, 150 };
				top_dl.rect( px, py, popup_w, popup_h, border_col, xdraw::corner_radius{ 6.0f } );

				// Hover effect
				if ( is_hovered )
				{
					const auto fill_col = detail::confirm_reset ? xdraw::color{ 255, 75, 85, 60 } : xdraw::color{ 255, 75, 85, 45 };
					top_dl.rect_filled( px + 2.0f, py + 2.0f, popup_w - 4.0f, popup_h - 4.0f, fill_col, xdraw::corner_radius{ 4.0f } );
				}

				const auto btn_text = detail::confirm_reset ? "confirm reset?" : "reset config";
				const auto text_col = is_danger ? xdraw::color{ 255, 85, 95 } : tokens::col_text;
				const auto [ tw, th ] = xdraw::measure_text( btn_text );
				top_dl.text( px + ( popup_w - tw ) * 0.5f, py + ( popup_h - th ) * 0.5f, btn_text, text_col );

				if ( is_hovered && input.mouse_clicked )
				{
					if ( !detail::confirm_reset )
					{
						detail::confirm_reset = true;
						detail::confirm_reset_timer = 0.0f;
					}
					else
					{
						detail::reset_defaults( );
						config::registry::save( detail::config_list[ detail::reset_target_idx ] );
						detail::reset_popup_open = false;
						detail::confirm_reset = false;
						detail::needs_refresh = true;
					}

					input.mouse_clicked = false;
				}
			}
		}

		// Fade the whole emitted panel as one unit on close / open.
		for ( std::size_t i = vtx_start; i < top_dl.vertices.size( ); ++i )
		{
			top_dl.vertices[ i ].col.a = static_cast< std::uint8_t >(
				static_cast< float >( top_dl.vertices[ i ].col.a ) * alpha_mult );
		}
	}

} // namespace rendering
