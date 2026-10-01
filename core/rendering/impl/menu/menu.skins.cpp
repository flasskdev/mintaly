#include <pch/pch.hpp>
#include <cstdio>
#include <iterator>
#include <commdlg.h>
#include <objbase.h>
#include <shobjidl.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <optional>
#include <core/features/features.hpp>
#include <core/systems/systems.hpp>
#include <core/settings.hpp>
#include <core/hooks/hooks.hpp>
#include <utilities/diag.hpp>

#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "shell32.lib")

#include "../../rendering.hpp"
#include "../../theme.hpp"
#include "skin_workspace.hpp"
#include "menu.chams_common.hpp"
#include <charconv>
#include <limits>

namespace rendering {

	namespace detail {

	struct pending_agent_file
	{
		int team{};
		std::string model_path{};
		std::string model_name{};
	};

	inline std::atomic<bool> s_dialog_active{ false };
	inline std::mutex s_agent_dialog_mutex{};
	inline std::vector<pending_agent_file> s_pending_agents{};

	static bool show_agent_dialog_seh( wchar_t* out_path, std::size_t max_path_len, int team )
	{
		bool success = false;
		__try
		{
			const HRESULT co_hr = CoInitializeEx( nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE );

			IFileOpenDialog* pFileOpen = nullptr;
			HRESULT hr = CoCreateInstance( __uuidof( FileOpenDialog ), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &pFileOpen ) );
			if ( SUCCEEDED( hr ) && pFileOpen )
			{
				static const COMDLG_FILTERSPEC rgSpec[] = {
					{ L"Model Files (*.vmdl;*.vmdl_c)", L"*.vmdl;*.vmdl_c" },
					{ L"All Files (*.*)", L"*.*" }
				};
				pFileOpen->SetFileTypes( ARRAYSIZE( rgSpec ), rgSpec );
				pFileOpen->SetTitle( ( team == 3 ) ? L"Select CT Custom Model (.vmdl)" : L"Select T Custom Model (.vmdl)" );

				DWORD dwFlags{};
				if ( SUCCEEDED( pFileOpen->GetOptions( &dwFlags ) ) )
				{
					pFileOpen->SetOptions( dwFlags | FOS_FORCEFILESYSTEM | FOS_FILEMUSTEXIST | FOS_NOCHANGEDIR | FOS_DONTADDTORECENT );
				}

				if ( SUCCEEDED( pFileOpen->Show( nullptr ) ) )
				{
					IShellItem* pItem = nullptr;
					if ( SUCCEEDED( pFileOpen->GetResult( &pItem ) ) && pItem )
					{
						PWSTR pszFilePath = nullptr;
						if ( SUCCEEDED( pItem->GetDisplayName( SIGDN_FILESYSPATH, &pszFilePath ) ) && pszFilePath )
						{
							wcsncpy_s( out_path, max_path_len, pszFilePath, _TRUNCATE );
							CoTaskMemFree( pszFilePath );
							success = true;
						}
						pItem->Release( );
					}
				}
				pFileOpen->Release( );
			}

			if ( !success && hr != HRESULT_FROM_WIN32( ERROR_CANCELLED ) )
			{
				wchar_t filename[ MAX_PATH ]{};
				OPENFILENAMEW ofn{};
				ofn.lStructSize = sizeof( ofn );
				ofn.hwndOwner = nullptr;
				ofn.lpstrFilter = L"Model Files (*.vmdl;*.vmdl_c)\0*.vmdl;*.vmdl_c\0All Files (*.*)\0*.*\0\0";
				ofn.lpstrFile = filename;
				ofn.nMaxFile = MAX_PATH;
				ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_DONTADDTORECENT;
				ofn.lpstrDefExt = L"vmdl";
				ofn.lpstrTitle = ( team == 3 ) ? L"Select CT Custom Model (.vmdl)" : L"Select T Custom Model (.vmdl)";

				if ( GetOpenFileNameW( &ofn ) )
				{
					wcsncpy_s( out_path, max_path_len, filename, _TRUNCATE );
					success = true;
				}
			}

			if ( SUCCEEDED( co_hr ) )
			{
				CoUninitialize( );
			}
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
			success = false;
		}

		return success;
	}

	static inline void process_pending_agents( )
	{
		std::vector<pending_agent_file> ready;
		{
			std::lock_guard lock( s_agent_dialog_mutex );
			if ( !s_pending_agents.empty( ) )
			{
				ready = std::move( s_pending_agents );
				s_pending_agents.clear( );
			}
		}

		if ( ready.empty( ) )
			return;

		auto& ca = settings::g_changer.custom_agents;
		for ( const auto& pa : ready )
		{
			if ( pa.model_path.empty( ) )
				continue;

			int existing_idx = -1;
			for ( int i = 0; i < static_cast< int >( ca.entries.size( ) ); ++i )
			{
				if ( ca.entries[ i ].model_path == pa.model_path )
				{
					existing_idx = i;
					break;
				}
			}

			if ( existing_idx < 0 )
			{
				settings::changer::custom_agent_entry entry;
				entry.name = pa.model_name;
				entry.model_path = pa.model_path;
				entry.team = pa.team;
				ca.entries.push_back( entry );
				existing_idx = static_cast< int >( ca.entries.size( ) ) - 1;
			}

			if ( pa.team == 3 )
			{
				ca.selected_ct = existing_idx;
				settings::g_changer.agents.ct_def = 0;
			}
			else
			{
				ca.selected_t = existing_idx;
				settings::g_changer.agents.t_def = 0;
			}

			diag::writef( diag::level::info, "custom agent applied: team=%d name=%s path=%s",
				pa.team, pa.model_name.c_str( ), pa.model_path.c_str( ) );
		}
	}

	static inline void open_agent_file_dialog( int team )
	{
		if ( s_dialog_active.exchange( true ) )
		{
			diag::write( diag::level::info, "agent file dialog already active, ignoring request" );
			return;
		}

		diag::writef( diag::level::info, "spawning agent file dialog worker for team %d", team );

		std::thread( [ team ]( )
		{
			struct raii_active
			{
				~raii_active( ) { s_dialog_active.store( false, std::memory_order_release ); }
			} active_guard;

			wchar_t filename[ MAX_PATH * 2 ]{};
			const bool selected = show_agent_dialog_seh( filename, std::size( filename ), team );

			if ( !selected || filename[ 0 ] == L'\0' )
			{
				diag::write( diag::level::info, "agent file dialog cancelled or failed" );
				return;
			}

			char path_utf8[ MAX_PATH * 2 ]{};
			const int utf8_len = WideCharToMultiByte( CP_UTF8, 0, filename, -1, path_utf8, static_cast< int >( sizeof( path_utf8 ) ), nullptr, nullptr );
			if ( utf8_len <= 1 )
			{
				return;
			}

			std::string model_path = path_utf8;
			std::replace( model_path.begin( ), model_path.end( ), '\\', '/' );
			std::string lower_path = model_path;
			for ( auto& c : lower_path ) {
				if ( c >= 'A' && c <= 'Z' ) c = static_cast<char>( c + 32 );
			}

			if ( auto p = lower_path.find( "game/csgo/" ); p != std::string::npos )
				model_path = model_path.substr( p + 10 );
			else if ( auto p = lower_path.find( "csgo/" ); p != std::string::npos )
				model_path = model_path.substr( p + 5 );
			else if ( auto p = lower_path.find( "characters/" ); p != std::string::npos )
				model_path = model_path.substr( p );
			else if ( auto p = lower_path.find( "models/" ); p != std::string::npos )
				model_path = model_path.substr( p );

			// CS2 engine requires .vmdl, not compiled .vmdl_c
			if ( model_path.size( ) >= 7 )
			{
				std::string ext = model_path.substr( model_path.size( ) - 7 );
				for ( auto& c : ext ) if ( c >= 'A' && c <= 'Z' ) c = static_cast<char>( c + 32 );
				if ( ext == ".vmdl_c" )
					model_path = model_path.substr( 0, model_path.size( ) - 2 );
			}

			std::string model_name = "Custom";
			auto last_slash = model_path.find_last_of( '/' );
			if ( last_slash != std::string::npos )
			{
				model_name = model_path.substr( last_slash + 1 );
				auto dot_pos = model_name.find_last_of( '.' );
				if ( dot_pos != std::string::npos )
				{
					model_name = model_name.substr( 0, dot_pos );
				}
			}

			auto to_l = []( unsigned char c ) { return ( c >= 'A' && c <= 'Z' ) ? static_cast< char >( c + 32 ) : static_cast< char >( c ); };
			auto bad_model = [ & ]( const char* n, std::size_t len )
			{
				if ( model_path.size( ) < len ) return false;
				for ( std::size_t i = 0; i + len <= model_path.size( ); ++i )
				{
					bool ok = true;
					for ( std::size_t j = 0; j < len; ++j )
						if ( to_l( static_cast< unsigned char >( model_path[ i + j ] ) ) != to_l( static_cast< unsigned char >( n[ j ] ) ) )
						{ ok = false; break; }
					if ( ok ) return true;
				}
				return false;
			};

			const bool is_arm = bad_model( "_arms.", 6 ) || bad_model( "/arms/", 6 ) || bad_model( "\\arms\\", 6 ) || bad_model( "viewmodel", 8 ) || bad_model( "/arm.", 5 ) || bad_model( "\\arm.", 5 );

			if ( !is_arm && !model_path.empty( ) )
			{
				std::lock_guard lock( s_agent_dialog_mutex );
				s_pending_agents.push_back( { team, std::move( model_path ), std::move( model_name ) } );
			}
		} ).detach( );
	}

		inline static auto& skin_map( ) { return settings::g_changer.skins.edit_team( skin_workspace::team ); }

		inline static void notify_skin_changed( )
		{
			features::changer::g_knives.invalidate( );
			features::changer::g_guns.invalidate( );
			features::changer::g_gloves.reset( );
			features::changer::g_agents.reset( );
			features::changer::g_skin_sync.trigger_push( );
		}

		enum class skins_page : int
		{
			grid,
			browser
		};

		enum class browser_tab : int
		{
			skins = 0,
			chams = 1
		};

		struct skins_state
		{
			skins_page current{ skins_page::grid };
			skins_page target{ skins_page::grid };
			float fade{ 1.0f };

			std::int16_t browsing_def{};
			int browsing_agent_team{};
			std::string search_buf{};
			browser_tab active_tab{ browser_tab::skins };
			bool details_open{};
		};

		skins_state skins_ui{};

		constexpr xdraw::color k_rarity_colors[ 8 ]
		{
			xdraw::color{ 235, 235, 235, 255 },
			xdraw::color{ 138, 173, 233, 255 },
			xdraw::color{  77, 116, 196, 255 },
			xdraw::color{ 138,  86, 207, 255 },
			xdraw::color{ 211,  44, 230, 255 },
			xdraw::color{ 235,  75,  75, 255 },
			xdraw::color{ 228, 174,  57, 255 },
			xdraw::color{ 255, 215,   0, 255 }
		};

		constexpr auto k_card_w_ref{ 118.0f };
		constexpr auto k_card_h_ref{ 100.0f };
		constexpr auto k_card_gap{ 8.0f };
		constexpr auto k_columns{ 5 };
		constexpr auto k_image_h_ratio{ 0.70f };
		constexpr auto k_rarity_bar_h{ 2.0f };

		static inline const char* wear_tier( float w )
		{
			return skin_options::wear_short[skin_options::wear_tier( w )];
		}

		static inline std::string format_wear( float w )
		{
			char buf[ 32 ]{};
			std::snprintf( buf, sizeof( buf ), "%.4f %s", w, wear_tier( w ) );
			return std::string{ buf };
		}

		static inline void request_page( skins_page p, std::int16_t def = 0 )
		{
			if ( skins_ui.target == p && ( p != skins_page::browser || skins_ui.browsing_def == def ) )
			{
				return;
			}

			skins_ui.target = p;
			skins_ui.fade = 1.0f;

			if ( p == skins_page::browser )
			{
				skins_ui.details_open = false;
				skins_ui.active_tab = browser_tab::skins;
				skins_ui.search_buf.clear( );
				skins_ui.browsing_def = def;
				skin_workspace::active_browsing_weapon = def;
				if ( def != 0 )
					skin_workspace::focused_weapon[skin_workspace::team == 3 ? 0 : 1] = def;
			}
			else
			{
				skin_workspace::active_browsing_weapon = 0;
				skins_ui.browsing_agent_team = 0;
			}
		}

		static inline const std::vector<const features::changer::econ_item_system::item_def*>& select_weapons( int subtab )
		{
			auto& econ = features::changer::g_econ_item_system;

			switch ( subtab )
			{
			case 1: return econ.knives( );
			case 2: return econ.gloves( );
			case 3: return econ.agents( );
			default: return econ.guns( );
			}
		}

		class wear_sub_overlay : public xui::overlay
		{
		public:
			wear_sub_overlay( std::uintptr_t id, const xui::rect& anchor, std::int16_t def ) : overlay{ id, anchor }, m_def{ def } {}

			[[nodiscard]] bool hit_test( float x, float y ) const override
			{
				return this->get_popup( ).contains( x, y );
			}

			bool process_input( const xui::input_state& input ) override
			{
				if ( this->m_closing )
				{
					return false;
				}

				const auto popup = this->get_popup( );
				if ( input.mouse_clicked && !popup.contains( input.mouse_x, input.mouse_y ) )
				{
					this->m_closing = true;
					return false;
				}

				return popup.contains( input.mouse_x, input.mouse_y );
			}

			void render( const xui::style& style, const xui::input_state& ) override
			{
				const auto dt = xdraw::delta_time( );
				const auto popup = this->get_popup( );

				const auto speed = this->m_closing ? 18.0f : 16.0f;
				const auto target = this->m_closing ? 0.0f : 1.0f;
				this->m_open_anim += ( target - this->m_open_anim ) * std::min( speed * dt, 1.0f );

				if ( this->m_open_anim < 0.01f && this->m_closing )
				{
					this->m_closed = true;
					return;
				}

				const auto et = xui::ease::out_cubic( this->m_open_anim );
				const auto alpha_mult = et;
				const auto animated_h = popup.h * et;
				const auto pr = style.popup_rounding;

				auto& dl = xdraw::get( xdraw::layer::top );

				auto bg = style.popup_bg;
				bg.a = static_cast< std::uint8_t >( bg.a * alpha_mult );
				auto border = xui::lighten( style.popup_border, 1.1f );
				border.a = static_cast< std::uint8_t >( border.a * alpha_mult );

				if ( !this->m_closing )
				{
					dl.rect_filled_blurred( popup.x, popup.y, popup.w, animated_h, xdraw::corner_radius{ pr } );
				}

				dl.rect_filled( popup.x, popup.y, popup.w, animated_h, bg, xdraw::corner_radius{ pr } );
				dl.rect( popup.x, popup.y, popup.w, animated_h, border, xdraw::corner_radius{ pr } );

				if ( et < 0.2f )
				{
					return;
				}

				xui::draw::push_layer( xdraw::layer::top );
				dl.push_clip( popup.x, popup.y, popup.w, animated_h );

				xui::window_state ws{};
				ws.title = "##wear_sub";
				ws.bounds = popup;
				ws.cursor_x = style.window_pad_x;
				ws.cursor_y = style.window_pad_y;
				ws.is_child = true;

				auto& c = xui::ctx( );
				c.windows.push_back( std::move( ws ) );
				xui::push_id( this->m_id );

				const auto prev_inside = c.inside_overlay;
				c.inside_overlay = this->m_id;

				const auto it = skin_map( ).find( this->m_def );
				if ( it != skin_map( ).end( ) )
				{
					const auto pk = features::changer::g_econ_item_system.find_paint_kit( it->second.paint_kit_id );
					const auto [low, high] = pk ? skin_options::wear_limits( pk->wear_min, pk->wear_max ) : std::pair{0.0f, 1.0f};
					it->second.wear = skin_options::clamp_wear( it->second.wear, low, high );
					if ( low < high ) xui::slider_float( "wear", it->second.wear, low, high, "%.6f" );
					else xui::text( std::format( "Fixed wear: {:.6f}", low ), style.text_dim );
				}

				c.inside_overlay = prev_inside;

				xui::pop_id( );
				c.windows.pop_back( );
				dl.pop_clip( );
				xui::draw::pop_layer( );
			}

		private:
			[[nodiscard]] xui::rect get_popup( ) const
			{
				return { this->m_anchor.x, this->m_anchor.y, 220.0f, 56.0f };
			}

			std::int16_t m_def{};
			float m_open_anim{};
		};

		class seed_sub_overlay : public xui::overlay
		{
		public:
			seed_sub_overlay( std::uintptr_t id, const xui::rect& anchor, std::int16_t def ) : overlay{ id, anchor }, m_def{ def }
			{
				const auto it = skin_map( ).find( def );
				if ( it != skin_map( ).end( ) )
				{
					this->m_buf = std::to_string( it->second.seed );
				}
			}

			[[nodiscard]] bool hit_test( float x, float y ) const override
			{
				return this->get_popup( ).contains( x, y );
			}

			bool process_input( const xui::input_state& input ) override
			{
				if ( this->m_closing )
				{
					return false;
				}

				const auto popup = this->get_popup( );
				if ( input.mouse_clicked && !popup.contains( input.mouse_x, input.mouse_y ) )
				{
					this->m_closing = true;
					return false;
				}

				return popup.contains( input.mouse_x, input.mouse_y );
			}

			void render( const xui::style& style, const xui::input_state& ) override
			{
				const auto dt = xdraw::delta_time( );
				const auto popup = this->get_popup( );

				const auto speed = this->m_closing ? 18.0f : 16.0f;
				const auto target = this->m_closing ? 0.0f : 1.0f;
				this->m_open_anim += ( target - this->m_open_anim ) * std::min( speed * dt, 1.0f );

				if ( this->m_open_anim < 0.01f && this->m_closing )
				{
					this->m_closed = true;
					return;
				}

				const auto et = xui::ease::out_cubic( this->m_open_anim );
				const auto alpha_mult = et;
				const auto animated_h = popup.h * et;
				const auto pr = style.popup_rounding;

				auto& dl = xdraw::get( xdraw::layer::top );

				auto bg = style.popup_bg;
				bg.a = static_cast< std::uint8_t >( bg.a * alpha_mult );
				auto border = xui::lighten( style.popup_border, 1.1f );
				border.a = static_cast< std::uint8_t >( border.a * alpha_mult );

				if ( !this->m_closing )
				{
					dl.rect_filled_blurred( popup.x, popup.y, popup.w, animated_h, xdraw::corner_radius{ pr } );
				}

				dl.rect_filled( popup.x, popup.y, popup.w, animated_h, bg, xdraw::corner_radius{ pr } );
				dl.rect( popup.x, popup.y, popup.w, animated_h, border, xdraw::corner_radius{ pr } );

				if ( et < 0.2f )
				{
					return;
				}

				xui::draw::push_layer( xdraw::layer::top );
				dl.push_clip( popup.x, popup.y, popup.w, animated_h );

				xui::window_state ws{};
				ws.title = "##seed_sub";
				ws.bounds = popup;
				ws.cursor_x = style.window_pad_x;
				ws.cursor_y = style.window_pad_y;
				ws.is_child = true;

				auto& c = xui::ctx( );
				c.windows.push_back( std::move( ws ) );
				xui::push_id( this->m_id );

				const auto prev_inside = c.inside_overlay;
				c.inside_overlay = this->m_id;

				if ( xui::text_input( "##seed_input", this->m_buf, 4, "0-1000" ) )
				{
					const auto it = skin_map( ).find( this->m_def );
					if ( it != skin_map( ).end( ) )
					{
						int value{};
						const auto result = std::from_chars(m_buf.data(), m_buf.data() + m_buf.size(), value);
						if (result.ec == std::errc{} && result.ptr == m_buf.data() + m_buf.size())
							it->second.seed = std::clamp(value, 0, 1000);
					}
				}

				c.inside_overlay = prev_inside;

				xui::pop_id( );
				c.windows.pop_back( );
				dl.pop_clip( );
				xui::draw::pop_layer( );
			}

		private:
			[[nodiscard]] xui::rect get_popup( ) const
			{
				return { this->m_anchor.x, this->m_anchor.y, 180.0f, 50.0f };
			}

			std::int16_t m_def{};
			std::string m_buf{};
			float m_open_anim{};
		};

		class skin_context_overlay : public xui::overlay
		{
		public:
			skin_context_overlay( std::uintptr_t id, const xui::rect& anchor, std::int16_t def, std::string weapon_name, std::string skin_name ) : overlay{ id, anchor }, m_def{ def }, m_weapon_name{ std::move( weapon_name ) }, m_skin_name{ std::move( skin_name ) }
			{
				this->m_hover_anims.fill( 0.0f );
				this->m_item_anims.fill( 0.0f );
			}

			[[nodiscard]] bool hit_test( float x, float y ) const override
			{
				return this->get_popup( ).contains( x, y );
			}

			bool process_input( const xui::input_state& input ) override
			{
				if ( this->m_closing )
				{
					return false;
				}

				if ( this->m_sub_id != xui::null_id )
				{
					if ( const auto sub = xui::overlays::find( this->m_sub_id ) )
					{
						if ( sub->hit_test( input.mouse_x, input.mouse_y ) )
						{
							return false;
						}
					}
					else
					{
						this->m_sub_id = xui::null_id;
					}
				}

				const auto popup = this->get_popup( );

				if ( ( input.mouse_clicked || input.rmb_clicked ) && !popup.contains( input.mouse_x, input.mouse_y ) )
				{
					this->m_closing = true;

					if ( this->m_sub_id != xui::null_id )
					{
						xui::overlays::close( this->m_sub_id );
					}

					return true;
				}

				if ( !input.mouse_clicked || !popup.contains( input.mouse_x, input.mouse_y ) )
				{
					return popup.contains( input.mouse_x, input.mouse_y );
				}

				const auto cur_count = this->get_count( );
				for ( auto i = 1; i < cur_count; ++i )
				{
					const auto ir = this->get_item_rect( popup, i );
					if ( !ir.contains( input.mouse_x, input.mouse_y ) )
					{
						continue;
					}

					if ( i == 1 )
					{
						request_page( skins_page::browser, this->m_def );
						skins_ui.active_tab = browser_tab::chams;
						this->m_closing = true;
						if ( this->m_sub_id != xui::null_id )
							xui::overlays::close( this->m_sub_id );
						return true;
					}

					const auto it = skin_map( ).find( this->m_def );
					if ( it == skin_map( ).end( ) )
					{
						if ( i == 2 )
						{
							request_page( skins_page::browser, this->m_def );
							skins_ui.active_tab = browser_tab::skins;
							this->m_closing = true;
							if ( this->m_sub_id != xui::null_id )
								xui::overlays::close( this->m_sub_id );
							return true;
						}
						return true;
					}

					if ( i == 2 )
					{
						const auto def = features::changer::g_econ_item_system.find_def( this->m_def );
						if ( def && ( def->category == features::changer::econ_item_system::item_category::gun || def->category == features::changer::econ_item_system::item_category::knife ) )
							it->second.stattrak = !it->second.stattrak;
					}
					else if ( i == 3 || i == 4 )
					{
						if ( this->m_sub_id != xui::null_id )
						{
							xui::overlays::close( this->m_sub_id );
						}

						const auto sub_anchor = xui::rect{ popup.right( ) + 4.0f, ir.y, 0.0f, 0.0f };
						this->m_sub_id = ( i == 3 ) ? ( this->m_id ^ 0x77711ull ) : ( this->m_id ^ 0x77722ull );

						if ( i == 3 )
						{
							xui::overlays::add( std::make_unique<wear_sub_overlay>( this->m_sub_id, sub_anchor, this->m_def ) );
						}
						else
						{
							xui::overlays::add( std::make_unique<seed_sub_overlay>( this->m_sub_id, sub_anchor, this->m_def ) );
						}
					}
					else if ( i == 5 )
					{
						skin_map( ).erase( this->m_def );
						notify_skin_changed( );

						this->m_closing = true;

						if ( this->m_sub_id != xui::null_id )
						{
							xui::overlays::close( this->m_sub_id );
						}
					}

					return true;
				}

				return true;
			}

			void render( const xui::style& style, const xui::input_state& input ) override
			{
				if ( this->m_sub_id != xui::null_id )
				{
					xui::overlays::touch( this->m_sub_id );
				}

				const auto dt = xdraw::delta_time( );
				const auto popup = this->get_popup( );
				auto& dl = xdraw::get( xdraw::layer::top );

				const auto speed = this->m_closing ? 18.0f : 16.0f;
				const auto target = this->m_closing ? 0.0f : 1.0f;
				this->m_open_anim += ( target - this->m_open_anim ) * std::min( speed * dt, 1.0f );

				if ( this->m_open_anim < 0.01f && this->m_closing )
				{
					this->m_closed = true;
					return;
				}

				const auto ease_t = xui::ease::out_cubic( this->m_open_anim );
				const auto alpha_mult = ease_t;
				const auto animated_h = popup.h * ease_t;
				const auto pr = style.popup_rounding;

				auto bg = style.popup_bg;
				bg.a = static_cast< std::uint8_t >( bg.a * alpha_mult );
				auto border = xui::lighten( style.popup_border, 1.1f );
				border.a = static_cast< std::uint8_t >( border.a * alpha_mult );

				if ( !this->m_closing )
				{
					dl.rect_filled_blurred( popup.x, popup.y, popup.w, animated_h, xdraw::corner_radius{ pr } );
				}

				dl.rect_filled( popup.x, popup.y, popup.w, animated_h, bg, xdraw::corner_radius{ pr } );
				dl.rect( popup.x, popup.y, popup.w, animated_h, border, xdraw::corner_radius{ pr } );

				dl.push_clip( popup.x, popup.y, popup.w, animated_h );

				const auto it = skin_map( ).find( this->m_def );
				const bool is_skinned = ( it != skin_map( ).end( ) );
				const auto cur_count = this->get_count( );

				for ( auto i = 0; i < cur_count; ++i )
				{
					const auto ir = this->get_item_rect( popup, i );
					const auto item_delay = i * 0.04f;
					const auto item_progress = std::clamp( ( this->m_open_anim - item_delay ) / ( 1.0f - std::min( item_delay, 0.3f ) ), 0.0f, 1.0f );
					auto& ia = this->m_item_anims[ i ];
					ia = std::min( ia + 20.0f * dt, item_progress );

					const auto item_ease = xui::ease::out_cubic( ia );
					const auto item_alpha = item_ease * alpha_mult;
					const auto slide = ( 1.0f - item_ease ) * 6.0f;

					if ( i == 0 )
					{
						char header[ 128 ]{};
						if ( !this->m_skin_name.empty( ) )
						{
							std::snprintf( header, sizeof( header ), "%s | %s", this->m_weapon_name.c_str( ), this->m_skin_name.c_str( ) );
						}
						else
						{
							std::snprintf( header, sizeof( header ), "%s", this->m_weapon_name.c_str( ) );
						}

						auto col = style.text;
						col.a = static_cast< std::uint8_t >( col.a * item_alpha );

						const auto trunc = xui::truncate( header, ir.w - 16.0f );
						const auto [tw, th] = xdraw::measure_text( trunc );
						dl.text( ir.x + 8.0f, ir.y + ( k_item_h - th ) * 0.5f + slide, trunc, col );
						continue;
					}

					const auto is_hovered = !this->m_closing && ir.contains( input.mouse_x, input.mouse_y );
					auto& ha = this->m_hover_anims[ i ];
					ha += ( ( is_hovered ? 1.0f : 0.0f ) - ha ) * std::min( 18.0f * dt, 1.0f );

					if ( ha > 0.01f )
					{
						auto hov = style.combo_popup_item_hovered;
						hov.a = static_cast< std::uint8_t >( hov.a * item_alpha * ha );
						const auto first = ( i == 1 );
						const auto last = ( i == cur_count - 1 );
						dl.rect_filled( ir.x, ir.y + slide, ir.w, ir.h, hov, xdraw::corner_radius{ first ? pr : 0.0f, first ? pr : 0.0f, last ? pr : 0.0f, last ? pr : 0.0f } );
					}

					if ( i == 1 )
					{
						auto label_col = style.text;
						label_col.a = static_cast< std::uint8_t >( label_col.a * item_alpha );

						const auto chams_idx = chams_weapons::index( this->m_def );
						std::string status = "default";
						if ( chams_idx >= 0 && settings::g_esp.m_viewmodel.individual.weapons[ chams_idx ].override_default.value )
							status = settings::g_esp.m_viewmodel.individual.weapons[ chams_idx ].cfg.enabled.value ? "custom" : "off";

						auto val_col = ( status == "custom" ) ? tokens::col_accent : style.text_dim;
						val_col.a = static_cast< std::uint8_t >( val_col.a * item_alpha );

						const auto [lw, lh] = xdraw::measure_text( "chams" );
						dl.text( ir.x + 8.0f, ir.y + ( k_item_h - lh ) * 0.5f + slide, "chams", label_col );

						const auto [vw, vh] = xdraw::measure_text( status );
						dl.text( ir.right( ) - vw - 8.0f, ir.y + ( k_item_h - vh ) * 0.5f + slide, status, val_col );
						continue;
					}

					if ( !is_skinned )
					{
						// i == 2: browse skins
						auto label_col = style.text;
						label_col.a = static_cast< std::uint8_t >( label_col.a * item_alpha );
						const auto [lw, lh] = xdraw::measure_text( "browse skins" );
						dl.text( ir.x + 8.0f, ir.y + ( k_item_h - lh ) * 0.5f + slide, "browse skins", label_col );

						auto val_col = tokens::col_accent;
						val_col.a = static_cast< std::uint8_t >( val_col.a * item_alpha );
						const auto [vw, vh] = xdraw::measure_text( ">" );
						dl.text( ir.right( ) - vw - 8.0f, ir.y + ( k_item_h - vh ) * 0.5f + slide, ">", val_col );
						continue;
					}

					if ( i == 5 )
					{
						// remove skin
						auto sep = style.separator;
						sep.a = static_cast< std::uint8_t >( sep.a * item_alpha );
						dl.line( ir.x + 6.0f, ir.y + slide, ir.right( ) - 6.0f, ir.y + slide, sep, 1.0f );

						auto col = xdraw::color{ 235, 75, 75, 230 };
						col = xui::lerp( col, xdraw::color{ 255, 100, 100, 255 }, ha );
						col.a = static_cast< std::uint8_t >( col.a * item_alpha );

						const auto [tw, th] = xdraw::measure_text( "remove skin" );
						dl.text( ir.x + 8.0f, ir.y + ( k_item_h - th ) * 0.5f + slide, "remove skin", col );
						continue;
					}

					static constexpr const char* labels[ ]{ "", "", "stattrak", "wear", "seed" };

					std::string value;
					if ( it != skin_map( ).end( ) )
					{
						if ( i == 2 )
						{
							value = it->second.stattrak ? "on" : "off";
						}
						else if ( i == 3 )
						{
							value = format_wear( it->second.wear );
						}
						else if ( i == 4 )
						{
							value = std::to_string( it->second.seed );
						}
					}
					else
					{
						value = "-";
					}

					auto label_col = style.text_dim;
					label_col.a = static_cast< std::uint8_t >( label_col.a * item_alpha );

					auto val_col = style.text;
					val_col = xui::lerp( val_col, xui::lighten( val_col, 1.3f ), ha );
					val_col.a = static_cast< std::uint8_t >( val_col.a * item_alpha );

					const auto [lw, lh] = xdraw::measure_text( labels[ i ] );
					dl.text( ir.x + 8.0f, ir.y + ( k_item_h - lh ) * 0.5f + slide, labels[ i ], label_col );

					const auto [vw, vh] = xdraw::measure_text( value );
					dl.text( ir.right( ) - vw - 8.0f, ir.y + ( k_item_h - vh ) * 0.5f + slide, value, val_col );
				}

				dl.pop_clip( );
			}

		private:
			[[nodiscard]] int get_count( ) const
			{
				const auto it = skin_map( ).find( this->m_def );
				return ( it != skin_map( ).end( ) ) ? 6 : 3;
			}
			static constexpr auto k_item_h{ 24.0f };
			static constexpr auto k_pad{ 4.0f };
			static constexpr auto k_popup_w{ 200.0f };

			[[nodiscard]] xui::rect get_popup( ) const
			{
				const auto h = k_pad * 2.0f + k_item_h * static_cast< float >( this->get_count( ) );
				return { this->m_anchor.x, this->m_anchor.y, k_popup_w, h };
			}

			[[nodiscard]] xui::rect get_item_rect( const xui::rect& popup, int index ) const
			{
				return { popup.x + k_pad, popup.y + k_pad + static_cast< float >( index ) * k_item_h, popup.w - k_pad * 2.0f, k_item_h };
			}

			std::int16_t m_def{};
			std::string m_weapon_name{};
			std::string m_skin_name{};
			std::uintptr_t m_sub_id{ xui::null_id };
			float m_open_anim{};
			std::array<float, 6> m_hover_anims{};
			std::array<float, 6> m_item_anims{};
		};

		static inline void draw_agent_team_card( const xui::rect& card, int team, float fade_alpha )
		{
			auto& econ = features::changer::g_econ_item_system;
			auto& dl = xui::draw::current( );
			const auto& input = xui::ctx( ).input;

			const auto& sel = settings::g_changer.agents;
			const auto selected_def = ( team == 3 ) ? sel.ct_def : sel.t_def;
			const auto def = ( selected_def != 0 ) ? econ.find_def( selected_def ) : nullptr;

			const features::changer::econ_item_system::item_def* default_agent{ nullptr };

			for ( const auto a : econ.agents( ) )
			{
				const auto agent_team = a->team( );
				if ( agent_team == team || agent_team == 0 )
				{
					default_agent = a;
					break;
				}
			}

			const auto image_h = std::floor( card.h * k_image_h_ratio );
			const auto hovered = !xui::ctx( ).overlay_blocking( ) && input.in_rect( card );
			const auto hover_anim = xui::anim::lerp( xui::fnv1a( "ateam" ) + static_cast< std::uintptr_t >( team ), hovered ? 1.0f : 0.0f, 14.0f );

			// Ambient drop shadow
			dl.rect_filled( card.x - 3.0f, card.y - 1.5f, card.w + 6.0f, card.h + 5.0f,
				xdraw::color{ 0, 0, 0, static_cast< std::uint8_t >( 24.0f * fade_alpha ) }, xdraw::corner_radius{ 10.0f } );
			dl.rect_filled( card.x, card.y + 1.5f, card.w, card.h,
				xdraw::color{ 0, 0, 0, static_cast< std::uint8_t >( 45.0f * fade_alpha ) }, xdraw::corner_radius{ 8.0f } );

			// 1. Frosted glass blur background
			dl.rect_filled_blurred( card.x, card.y, card.w, card.h, xdraw::corner_radius{ 8.0f },
				xdraw::color{ 160, 170, 190, static_cast< std::uint8_t >( 55.0f * fade_alpha ) } );

			// 2. Translucent frosted body with whitish tint on hover
			const auto card_top = xdraw::color{ 34, 38, 50, static_cast< std::uint8_t >( ( 125.0f + 30.0f * hover_anim ) * fade_alpha ) };
			const auto card_bot = xdraw::color{ 20, 23, 30, static_cast< std::uint8_t >( ( 150.0f + 30.0f * hover_anim ) * fade_alpha ) };
			dl.rect_filled_gradient( card.x, card.y, card.w, card.h, card_top, card_top, card_bot, card_bot, xdraw::corner_radius{ 8.0f } );

			// 3. Specular top rim
			dl.line( card.x + 8.0f, card.y + 0.5f, card.x + card.w - 8.0f, card.y + 0.5f,
				xdraw::color{ 255, 255, 255, static_cast< std::uint8_t >( ( 25.0f + 35.0f * hover_anim ) * fade_alpha ) }, 1.0f );

			// 4. Border
			auto bcol = def ? tokens::col_accent : xdraw::color{ 255, 255, 255, 25 };
			bcol.a = static_cast< std::uint8_t >( ( def ? ( 130.0f + 80.0f * hover_anim ) : ( 25.0f + 55.0f * hover_anim ) ) * fade_alpha );
			dl.rect( card.x, card.y, card.w, card.h, bcol, xdraw::corner_radius{ 8.0f }, def ? 1.5f : 1.0f );

			const auto img_source = def ? def : default_agent;
			if ( img_source )
			{
				const auto img = econ.get_skin_image( img_source->image_inventory );
				if ( img )
				{
					const auto target_h = image_h - 12.0f;
					const auto aspect = static_cast< float >( img->width ) / static_cast< float >( img->height );
					auto iw = target_h * aspect;
					auto ih = target_h;

					if ( iw > card.w - 14.0f )
					{
						iw = card.w - 14.0f;
						ih = iw / aspect;
					}

					const auto ix = std::floor( card.x + ( card.w - iw ) * 0.5f );
					const auto iy = std::floor( card.y + ( image_h - ih ) * 0.5f - hover_anim * 2.0f );
					const auto alpha = def ? 255.0f : 120.0f;
					const auto tint = xdraw::color{ 255, 255, 255, static_cast< std::uint8_t >( alpha * fade_alpha ) };

					dl.image( ix, iy, iw, ih, img->srv.Get( ), tint );
				}
			}

			const auto name_y = card.y + image_h + k_rarity_bar_h + 4.0f;
			std::string label = def ? def->localized_name : ( team == 3 ? "CT" : "T" );
			{
				const auto& ca = settings::g_changer.custom_agents;
				const auto cidx = ( team == 3 ) ? ca.selected_ct : ca.selected_t;
				if ( cidx >= 0 && cidx < static_cast< int >( ca.entries.size( ) ) )
				{
					label = ca.entries[ cidx ].name;
				}
			}

			auto ncol = xui::lerp( tokens::col_text_dim, tokens::col_text, hover_anim );
			ncol.a = static_cast< std::uint8_t >( ncol.a * fade_alpha );

			const auto ntrunc = xui::truncate( label, card.w - 12.0f );
			const auto [nw, nh] = xdraw::measure_text( ntrunc );
			dl.text( std::floor( card.x + ( card.w - nw ) * 0.5f ), std::floor( name_y ), ntrunc, ncol );

			if ( def )
			{
				const auto team_label = ( team == 3 ) ? "CT" : "T";
				auto team_col = tokens::col_text_dim;
				team_col.a = static_cast< std::uint8_t >( 180.0f * fade_alpha );
				dl.text( card.x + 6.0f, card.y + 4.0f, team_label, team_col );
			}

			// Add custom agent button (+)
			constexpr auto plus_btn_size{ 20.0f };
			const auto plus_x = card.right( ) - plus_btn_size - 6.0f;
			const auto plus_y = card.y + 6.0f;
			const auto plus_rect = xui::rect{ plus_x, plus_y, plus_btn_size, plus_btn_size };
			const auto plus_hovered = !xui::ctx( ).overlay_blocking( ) && input.in_rect( plus_rect );
			const auto plus_hover_anim = xui::anim::lerp( xui::fnv1a( "aplus" ) + static_cast< std::uintptr_t >( team ), plus_hovered ? 1.0f : 0.0f, 14.0f );

			auto plus_bg = tokens::col_accent;
			plus_bg.a = static_cast< std::uint8_t >( ( 180.0f + 75.0f * plus_hover_anim ) * fade_alpha );
			dl.rect_filled( plus_rect.x, plus_rect.y, plus_rect.w, plus_rect.h, plus_bg, xdraw::corner_radius{ 4.0f } );

			const auto plus_text_col = xdraw::color{ 255, 255, 255, static_cast< std::uint8_t >( 255.0f * fade_alpha ) };
			const auto plus_text = s_dialog_active ? "..." : "+";
			const auto [pw, ph] = xdraw::measure_text( plus_text );
			dl.text( plus_rect.x + ( plus_rect.w - pw ) * 0.5f, plus_rect.y + ( plus_rect.h - ph ) * 0.5f, plus_text, plus_text_col );

			// Reset custom agent button (показываем только если кастом выбран для этой команды)
			bool reset_hovered = false;
			{
				const auto& ca_now = settings::g_changer.custom_agents;
				const auto sel_idx = ( team == 3 ) ? ca_now.selected_ct : ca_now.selected_t;
				const auto sel_valid = sel_idx >= 0 && sel_idx < static_cast< int >( ca_now.entries.size( ) )
				                    && ( ca_now.entries[ sel_idx ].team == team || ca_now.entries[ sel_idx ].team == 0 );

				if ( sel_valid )
				{
					constexpr auto reset_btn_size{ 20.0f };
					const auto reset_x = plus_x - reset_btn_size - 4.0f;
					const auto reset_y = card.y + 6.0f;
					const auto reset_rect = xui::rect{ reset_x, reset_y, reset_btn_size, reset_btn_size };
					reset_hovered = !xui::ctx( ).overlay_blocking( ) && input.in_rect( reset_rect );
					const auto reset_hover_anim = xui::anim::lerp( xui::fnv1a( "areset" ) + static_cast< std::uintptr_t >( team ), reset_hovered ? 1.0f : 0.0f, 14.0f );

					auto reset_bg = xui::lerp( tokens::col_card, xui::lighten( tokens::col_card, 1.3f ), reset_hover_anim );
					reset_bg.a = static_cast< std::uint8_t >( ( 170.0f + 85.0f * reset_hover_anim ) * fade_alpha );
					dl.rect_filled( reset_rect.x, reset_rect.y, reset_rect.w, reset_rect.h, reset_bg, xdraw::corner_radius{ 4.0f } );

					auto reset_col = xdraw::color{ 230, 90, 90, static_cast< std::uint8_t >( 255.0f * fade_alpha ) };
					const auto [xw, xh] = xdraw::measure_text( "x" );
					dl.text( reset_rect.x + ( reset_rect.w - xw ) * 0.5f, reset_rect.y + ( reset_rect.h - xh ) * 0.5f, "x", reset_col );

					if ( reset_hovered && input.mouse_clicked )
					{
						auto& ca = settings::g_changer.custom_agents;
						if ( team == 3 )
							ca.selected_ct = -1;
						else
							ca.selected_t = -1;
					}
				}
			}

			// Check for custom models
			const auto& custom_agents = settings::g_changer.custom_agents;
			const auto has_custom = !custom_agents.entries.empty( );

			// Show custom models indicator if any
			if ( has_custom )
			{
				const auto custom_count = static_cast< int >( custom_agents.entries.size( ) );
				const auto idx = ( team == 3 ) ? custom_agents.selected_ct : custom_agents.selected_t;

				if ( idx >= 0 && idx < custom_count )
				{
					const auto& entry = custom_agents.entries[ idx ];
					if ( entry.team == team || entry.team == 0 )
					{
						auto custom_col = tokens::col_accent;
						custom_col.a = static_cast< std::uint8_t >( 200.0f * fade_alpha );
						dl.text( card.x + 6.0f, card.y + card.h - 16.0f, "*custom", custom_col );
					}
				}
			}

			if ( plus_hovered && input.mouse_clicked )
			{
				open_agent_file_dialog( team );
			}
			else if ( hovered && input.mouse_clicked && !plus_hovered && !reset_hovered )
			{
				skins_ui.browsing_agent_team = team;
				request_page( skins_page::browser, 0 );
			}
		}

		static inline void draw_weapon_card( const xui::rect& card, const features::changer::econ_item_system::item_def* def, float fade_alpha )
		{
			auto& econ = features::changer::g_econ_item_system;
			auto& dl = xui::draw::current( );
			const auto& input = xui::ctx( ).input;

			const auto applied_it = skin_map( ).find( def->def_index );
			const auto is_skinned = ( applied_it != skin_map( ).end( ) );

			const auto pk = ( is_skinned && applied_it->second.paint_kit_id != 0 ) ? econ.find_paint_kit( applied_it->second.paint_kit_id ) : nullptr;
			const auto rarity = pk ? econ.combined_rarity( def->def_index, applied_it->second.paint_kit_id ) : 0;
			const auto rarity_col = k_rarity_colors[ std::clamp( rarity, 0, 7 ) ];

			const auto image_h = std::floor( card.h * k_image_h_ratio );

			const auto hovered = skin_workspace::hovered(card);
			if ( hovered && input.mouse_clicked )
				skin_workspace::focused_weapon[ skin_workspace::team == 3 ? 0 : 1 ] = def->def_index;
			const auto hover_anim = xui::anim::lerp( xui::fnv1a( "wcard" ) + static_cast< std::uintptr_t >( def->def_index ), hovered ? 1.0f : 0.0f, 14.0f );

			// Ambient drop shadow
			dl.rect_filled( card.x - 3.0f, card.y - 1.5f, card.w + 6.0f, card.h + 5.0f,
				xdraw::color{ 0, 0, 0, static_cast< std::uint8_t >( 24.0f * fade_alpha ) }, xdraw::corner_radius{ 10.0f } );
			dl.rect_filled( card.x, card.y + 1.5f, card.w, card.h,
				xdraw::color{ 0, 0, 0, static_cast< std::uint8_t >( 45.0f * fade_alpha ) }, xdraw::corner_radius{ 8.0f } );

			// 1. Frosted glass blur background
			dl.rect_filled_blurred( card.x, card.y, card.w, card.h, xdraw::corner_radius{ 8.0f },
				xdraw::color{ 160, 170, 190, static_cast< std::uint8_t >( 55.0f * fade_alpha ) } );

			// 2. Translucent frosted body with subtle dark tone
			const auto card_top = xdraw::color{ 22, 25, 34, static_cast< std::uint8_t >( ( 140.0f + 25.0f * hover_anim ) * fade_alpha ) };
			const auto card_bot = xdraw::color{ 14, 16, 22, static_cast< std::uint8_t >( ( 165.0f + 25.0f * hover_anim ) * fade_alpha ) };
			dl.rect_filled_gradient( card.x, card.y, card.w, card.h, card_top, card_top, card_bot, card_bot, xdraw::corner_radius{ 8.0f } );

			// 3. Subtle ambient rarity glow
			if ( pk )
			{
				const auto glow = rarity_col.alpha( static_cast< std::uint8_t >( ( 14.0f + 28.0f * hover_anim ) * fade_alpha ) );
				dl.rect_filled_gradient( card.x, card.y + card.h * 0.45f, card.w, card.h * 0.55f,
					glow.alpha( 0 ), glow.alpha( 0 ), glow, glow, xdraw::corner_radius::bottom( 8.0f ) );
			}

			// 4. Specular top rim
			dl.line( card.x + 8.0f, card.y + 0.5f, card.x + card.w - 8.0f, card.y + 0.5f,
				xdraw::color{ 255, 255, 255, static_cast< std::uint8_t >( ( 25.0f + 35.0f * hover_anim ) * fade_alpha ) }, 1.0f );

			// 5. Border
			auto bcol = is_skinned ? ( pk ? rarity_col : tokens::col_accent ) : xdraw::color{ 255, 255, 255, 25 };
			bcol.a = static_cast< std::uint8_t >( ( is_skinned ? ( 120.0f + 80.0f * hover_anim ) : ( 25.0f + 55.0f * hover_anim ) ) * fade_alpha );
			dl.rect( card.x, card.y, card.w, card.h, bcol, xdraw::corner_radius{ 8.0f }, is_skinned ? 1.5f : 1.0f );

			const auto img = ( is_skinned && pk ) ? econ.get_skin_image( def->def_index, applied_it->second.paint_kit_id ) : econ.get_skin_image( def->def_index, 0 );
			if ( img )
			{
				const auto target_h = image_h - 12.0f;
				const auto aspect = static_cast< float >( img->width ) / static_cast< float >( img->height );
				auto iw = target_h * aspect;
				auto ih = target_h;

				if ( iw > card.w - 14.0f )
				{
					iw = card.w - 14.0f;
					ih = iw / aspect;
				}

				const auto ix = std::floor( card.x + ( card.w - iw ) * 0.5f );
				const auto iy = std::floor( card.y + ( image_h - ih ) * 0.5f - hover_anim * 2.0f );
				const auto tint = xdraw::color{ 255, 255, 255, static_cast< std::uint8_t >( 255.0f * fade_alpha ) };

				dl.image( ix, iy, iw, ih, img->srv.Get( ), tint );
			}
			else if ( def->category == features::changer::econ_item_system::item_category::glove && !img )
			{
				for ( const auto& skin : econ.skins( ) )
				{
					if ( skin.def_index != def->def_index )
					{
						continue;
					}

					const auto fallback_img = econ.get_skin_image( def->def_index, skin.paint_kit_id );
					if ( fallback_img )
					{
						const auto target_h = image_h - 12.0f;
						const auto aspect = static_cast< float >( fallback_img->width ) / static_cast< float >( fallback_img->height );
						auto iw = target_h * aspect;
						auto ih = target_h;

						if ( iw > card.w - 14.0f )
						{
							iw = card.w - 14.0f;
							ih = iw / aspect;
						}

						const auto ix = std::floor( card.x + ( card.w - iw ) * 0.5f );
						const auto iy = std::floor( card.y + ( image_h - ih ) * 0.5f - hover_anim * 2.0f );
						const auto tint = xdraw::color{ 255, 255, 255, static_cast< std::uint8_t >( 150.0f * fade_alpha ) };

						dl.image( ix, iy, iw, ih, fallback_img->srv.Get( ), tint );
						break;
					}
				}
			}

			if ( pk )
			{
				auto bar = rarity_col;
				bar.a = static_cast< std::uint8_t >( ( 180.0f + 75.0f * hover_anim ) * fade_alpha );
				dl.rect_filled( card.x + 8.0f, card.bottom( ) - 3.5f, card.w - 16.0f, 2.5f, bar, xdraw::corner_radius{ 1.25f } );
			}

			const auto name_y = card.y + image_h + 4.0f;

			if ( pk )
			{
				auto skin_col = rarity_col;
				skin_col.a = static_cast< std::uint8_t >( 200.0f * fade_alpha );

				const auto skin_trunc = xui::truncate( pk->localized_name, card.w - 12.0f );
				const auto [sw, sh] = xdraw::measure_text( skin_trunc );
				dl.text( std::floor( card.x + ( card.w - sw ) * 0.5f ), std::floor( name_y ), skin_trunc, skin_col );
			}
			else
			{
				auto ncol = xui::lerp( tokens::col_text_dim, tokens::col_text, hover_anim );
				ncol.a = static_cast< std::uint8_t >( ncol.a * fade_alpha );

				const auto ntrunc = xui::truncate( def->localized_name, card.w - 12.0f );
				const auto [nw2, nh2] = xdraw::measure_text( ntrunc );
				dl.text( std::floor( card.x + ( card.w - nw2 ) * 0.5f ), std::floor( name_y ), ntrunc, ncol );
			}

			if ( !hovered )
			{
				return;
			}

			if ( input.mouse_clicked )
			{
				if ( def->category == features::changer::econ_item_system::item_category::agent )
				{
					if ( is_skinned )
					{
						skin_map( ).erase( def->def_index );
					}
					else
					{
						for ( const auto* a : econ.agents( ) )
						{
							skin_map( ).erase( a->def_index );
						}

						auto& a = skin_map( )[ def->def_index ];
						a.paint_kit_id = 0;
						a.wear = 0.0f;
						a.seed = 0;
						a.stattrak = false;
					}
					notify_skin_changed( );
				}
				else
				{
					request_page( skins_page::browser, def->def_index );
				}

				return;
			}

			if ( input.rmb_clicked )
			{
				const auto ctx_id = xui::fnv1a( "skin_ctx" ) ^ static_cast< std::uintptr_t >( def->def_index );

				if ( xui::overlays::is_open( ctx_id ) )
				{
					xui::overlays::close( ctx_id );
				}
				else
				{
					const auto anchor = xui::rect{ input.mouse_x, input.mouse_y, 0.0f, 0.0f };
					const std::string skin_name = ( is_skinned && pk ) ? pk->localized_name : "";
					xui::overlays::add( std::make_unique<skin_context_overlay>( ctx_id, anchor, def->def_index, def->localized_name, skin_name ) );
				}
			}
		}

		static inline void draw_custom_agent_tile( const xui::rect& card, int entry_idx, const settings::changer::custom_agent_entry& entry, bool is_equipped, float fade_alpha )
		{
			auto& dl = xui::draw::current( );
			const auto& input = xui::ctx( ).input;

			const auto image_h = std::floor( card.h * k_image_h_ratio );
			const auto hovered = skin_workspace::hovered(card);
			const auto hover_anim = xui::anim::lerp( xui::fnv1a( "catile" ) + static_cast< std::uintptr_t >( entry_idx ), hovered ? 1.0f : 0.0f, 14.0f );

			auto card_bg = tokens::col_elevated;
			card_bg = xui::lerp( card_bg, xui::lighten( card_bg, 1.4f ), hover_anim * 0.5f );
			card_bg.a = static_cast< std::uint8_t >( card_bg.a * fade_alpha );
			dl.rect_filled( card.x, card.y, card.w, card.h, card_bg, xdraw::corner_radius{ tokens::btn_rounding } );

			if ( is_equipped )
			{
				auto bcol = tokens::col_accent;
				bcol.a = static_cast< std::uint8_t >( bcol.a * fade_alpha );
				dl.rect( card.x, card.y, card.w, card.h, bcol, xdraw::corner_radius{ tokens::btn_rounding }, 1.5f );
			}

			// Tag / label in center of image area
			{
				auto tag_col = tokens::col_accent;
				tag_col.a = static_cast< std::uint8_t >( 200.0f * fade_alpha );
				const auto [tw, th] = xdraw::measure_text( ".VMDL" );
				dl.text( std::floor( card.x + ( card.w - tw ) * 0.5f ), std::floor( card.y + ( image_h - th ) * 0.5f ), ".VMDL", tag_col );
			}

			const auto name_y = card.y + image_h + k_rarity_bar_h + 4.0f;
			auto ncol = xui::lerp( tokens::col_text_dim, tokens::col_text, hover_anim );
			ncol.a = static_cast< std::uint8_t >( ncol.a * fade_alpha );

			const auto ntrunc = xui::truncate( entry.name.empty( ) ? "Custom Agent" : entry.name, card.w - 12.0f );
			const auto [nw, nh] = xdraw::measure_text( ntrunc );
			dl.text( std::floor( card.x + ( card.w - nw ) * 0.5f ), std::floor( name_y ), ntrunc, ncol );

			// Delete button (top-right 'x')
			constexpr auto del_btn_size{ 16.0f };
			const auto del_rect = xui::rect{ card.right( ) - del_btn_size - 4.0f, card.y + 4.0f, del_btn_size, del_btn_size };
			const auto del_hovered = skin_workspace::hovered(del_rect);
			if ( del_hovered )
			{
				auto del_col = xdraw::color{ 230, 90, 90, static_cast< std::uint8_t >( 255.0f * fade_alpha ) };
				const auto [dw, dh] = xdraw::measure_text( "x" );
				dl.text( del_rect.x + ( del_rect.w - dw ) * 0.5f, del_rect.y + ( del_rect.h - dh ) * 0.5f, "x", del_col );
			}

			if ( is_equipped )
			{
				constexpr auto badge{ 14.0f };
				const auto bx = card.x + 4.0f;
				const auto by = card.y + 4.0f;

				auto badge_bg = tokens::col_accent;
				badge_bg.a = static_cast< std::uint8_t >( badge_bg.a * fade_alpha );
				dl.rect_filled( bx, by, badge, badge, badge_bg, xdraw::corner_radius{ badge * 0.5f } );

				const auto cx = bx + badge * 0.5f;
				const auto cy = by + badge * 0.5f;
				const auto check = xdraw::color{ tokens::col_dark.r, tokens::col_dark.g, tokens::col_dark.b, static_cast< std::uint8_t >( 255.0f * fade_alpha ) };

				const std::array<float, 6> pts
				{
					cx - badge * 0.20f, cy,
					cx - badge * 0.05f, cy + badge * 0.18f,
					cx + badge * 0.25f, cy - badge * 0.18f
				};

				dl.polyline( pts, check, false, 1.5f );
			}

			if ( del_hovered && input.mouse_clicked )
			{
				auto& ca = settings::g_changer.custom_agents;
				if ( entry_idx >= 0 && entry_idx < static_cast< int >( ca.entries.size( ) ) )
				{
					skin_workspace::pending_delete_agent = entry_idx;
				}
				return;
			}

			if ( hovered && input.mouse_clicked && !del_hovered )
			{
				auto& ca = settings::g_changer.custom_agents;
				auto& target = ( skins_ui.browsing_agent_team == 3 ) ? settings::g_changer.agents.ct_def : settings::g_changer.agents.t_def;
				auto& sel_idx = ( skins_ui.browsing_agent_team == 3 ) ? ca.selected_ct : ca.selected_t;

				if ( is_equipped )
				{
					sel_idx = -1;
				}
				else
				{
					sel_idx = entry_idx;
					target = 0; // custom agent overrides official agent
				}
				notify_skin_changed( );
			}
		}

		static inline void draw_agent_tile( const xui::rect& card, const features::changer::econ_item_system::item_def* def, bool is_equipped, float fade_alpha )
		{
			auto& econ = features::changer::g_econ_item_system;
			auto& dl = xui::draw::current( );
			const auto& input = xui::ctx( ).input;

			const auto image_h = std::floor( card.h * k_image_h_ratio );

			const auto hovered = skin_workspace::hovered(card);
			const auto hover_anim = xui::anim::lerp( xui::fnv1a( "atile" ) + static_cast< std::uintptr_t >( def->def_index ), hovered ? 1.0f : 0.0f, 14.0f );

			// Ambient drop shadow
			dl.rect_filled( card.x - 3.0f, card.y - 1.5f, card.w + 6.0f, card.h + 5.0f,
				xdraw::color{ 0, 0, 0, static_cast< std::uint8_t >( 24.0f * fade_alpha ) }, xdraw::corner_radius{ 10.0f } );
			dl.rect_filled( card.x, card.y + 1.5f, card.w, card.h,
				xdraw::color{ 0, 0, 0, static_cast< std::uint8_t >( 45.0f * fade_alpha ) }, xdraw::corner_radius{ 8.0f } );

			// 1. Frosted glass blur background
			dl.rect_filled_blurred( card.x, card.y, card.w, card.h, xdraw::corner_radius{ 8.0f },
				xdraw::color{ 160, 170, 190, static_cast< std::uint8_t >( 55.0f * fade_alpha ) } );

			// 2. Translucent frosted body with subtle dark tone
			const auto card_top = xdraw::color{ 22, 25, 34, static_cast< std::uint8_t >( ( 140.0f + 25.0f * hover_anim ) * fade_alpha ) };
			const auto card_bot = xdraw::color{ 14, 16, 22, static_cast< std::uint8_t >( ( 165.0f + 25.0f * hover_anim ) * fade_alpha ) };
			dl.rect_filled_gradient( card.x, card.y, card.w, card.h, card_top, card_top, card_bot, card_bot, xdraw::corner_radius{ 8.0f } );

			// 3. Specular top rim
			dl.line( card.x + 8.0f, card.y + 0.5f, card.x + card.w - 8.0f, card.y + 0.5f,
				xdraw::color{ 255, 255, 255, static_cast< std::uint8_t >( ( 25.0f + 35.0f * hover_anim ) * fade_alpha ) }, 1.0f );

			// 4. Border
			auto bcol = is_equipped ? tokens::col_accent : xdraw::color{ 255, 255, 255, 25 };
			bcol.a = static_cast< std::uint8_t >( ( is_equipped ? ( 160.0f + 80.0f * hover_anim ) : ( 25.0f + 55.0f * hover_anim ) ) * fade_alpha );
			dl.rect( card.x, card.y, card.w, card.h, bcol, xdraw::corner_radius{ 8.0f }, is_equipped ? 1.5f : 1.0f );

			const auto img = econ.get_skin_image( def->image_inventory );
			if ( img )
			{
				const auto target_h = image_h - 12.0f;
				const auto aspect = static_cast< float >( img->width ) / static_cast< float >( img->height );
				auto iw = target_h * aspect;
				auto ih = target_h;

				if ( iw > card.w - 14.0f )
				{
					iw = card.w - 14.0f;
					ih = iw / aspect;
				}

				const auto ix = std::floor( card.x + ( card.w - iw ) * 0.5f );
				const auto iy = std::floor( card.y + ( image_h - ih ) * 0.5f - hover_anim * 2.0f );
				const auto tint = xdraw::color{ 255, 255, 255, static_cast< std::uint8_t >( 255.0f * fade_alpha ) };

				dl.image( ix, iy, iw, ih, img->srv.Get( ), tint );
			}
			else
			{
				auto tag_col = tokens::col_text_dim;
				tag_col.a = static_cast< std::uint8_t >( 120.0f * fade_alpha );
				const auto [tw, th] = xdraw::measure_text( "AGENT" );
				dl.text( std::floor( card.x + ( card.w - tw ) * 0.5f ), std::floor( card.y + ( image_h - th ) * 0.5f ), "AGENT", tag_col );
			}

			const auto name_y = card.y + image_h + k_rarity_bar_h + 4.0f;

			auto ncol = xui::lerp( tokens::col_text_dim, tokens::col_text, hover_anim );
			ncol.a = static_cast< std::uint8_t >( ncol.a * fade_alpha );

			const auto ntrunc = xui::truncate( def->localized_name, card.w - 12.0f );
			const auto [nw, nh] = xdraw::measure_text( ntrunc );
			dl.text( std::floor( card.x + ( card.w - nw ) * 0.5f ), std::floor( name_y ), ntrunc, ncol );

			if ( is_equipped )
			{
				constexpr auto badge{ 14.0f };
				const auto bx = card.right( ) - badge - 4.0f;
				const auto by = card.y + 4.0f;

				auto badge_bg = tokens::col_accent;
				badge_bg.a = static_cast< std::uint8_t >( badge_bg.a * fade_alpha );
				dl.rect_filled( bx, by, badge, badge, badge_bg, xdraw::corner_radius{ badge * 0.5f } );

				const auto cx = bx + badge * 0.5f;
				const auto cy = by + badge * 0.5f;
				const auto check = xdraw::color{ tokens::col_dark.r, tokens::col_dark.g, tokens::col_dark.b, static_cast< std::uint8_t >( 255.0f * fade_alpha ) };

				const std::array<float, 6> pts
				{
					cx - badge * 0.20f, cy,
					cx - badge * 0.05f, cy + badge * 0.18f,
					cx + badge * 0.25f, cy - badge * 0.18f
				};

				dl.polyline( pts, check, false, 1.5f );
			}

			if ( hovered && input.mouse_clicked )
			{
				auto& target = ( skins_ui.browsing_agent_team == 3 ) ? settings::g_changer.agents.ct_def : settings::g_changer.agents.t_def;
				auto& ca = settings::g_changer.custom_agents;

				if ( is_equipped )
				{
					target = 0;
				}
				else
				{
					target = def->def_index;
					if ( skins_ui.browsing_agent_team == 3 )
						ca.selected_ct = -1;
					else
						ca.selected_t = -1;
				}
				notify_skin_changed( );
			}
		}

		static inline void draw_music_tile( const xui::rect& card, const features::changer::econ_item_system::music_kit* kit, bool is_equipped, float fade_alpha )
		{
			auto& econ = features::changer::g_econ_item_system;
			auto& dl = xui::draw::current( );
			const auto& input = xui::ctx( ).input;

			const auto image_h = std::floor( card.h * k_image_h_ratio );

			const auto hovered = skin_workspace::hovered(card);
			const auto hover_id = xui::fnv1a( "mtile" ) + ( kit ? static_cast< std::uintptr_t >( kit->id ) : 0 );
			const auto hover_anim = xui::anim::lerp( hover_id, hovered ? 1.0f : 0.0f, 14.0f );

			const auto r_idx = kit ? std::clamp( static_cast< int >( kit->rarity ), 0, 7 ) : 1;
			const auto rarity_col = k_rarity_colors[ r_idx ];

			// Ambient drop shadow
			dl.rect_filled( card.x - 3.0f, card.y - 1.5f, card.w + 6.0f, card.h + 5.0f,
				xdraw::color{ 0, 0, 0, static_cast< std::uint8_t >( 24.0f * fade_alpha ) }, xdraw::corner_radius{ 10.0f } );
			dl.rect_filled( card.x, card.y + 1.5f, card.w, card.h,
				xdraw::color{ 0, 0, 0, static_cast< std::uint8_t >( 45.0f * fade_alpha ) }, xdraw::corner_radius{ 8.0f } );

			// 1. Frosted glass blur background
			dl.rect_filled_blurred( card.x, card.y, card.w, card.h, xdraw::corner_radius{ 8.0f },
				xdraw::color{ 160, 170, 190, static_cast< std::uint8_t >( 55.0f * fade_alpha ) } );

			// 2. Translucent frosted body with subtle dark tone
			const auto card_top = xdraw::color{ 22, 25, 34, static_cast< std::uint8_t >( ( 140.0f + 25.0f * hover_anim ) * fade_alpha ) };
			const auto card_bot = xdraw::color{ 14, 16, 22, static_cast< std::uint8_t >( ( 165.0f + 25.0f * hover_anim ) * fade_alpha ) };
			dl.rect_filled_gradient( card.x, card.y, card.w, card.h, card_top, card_top, card_bot, card_bot, xdraw::corner_radius{ 8.0f } );

			// 3. Ambient rarity glow
			if ( kit )
			{
				const auto glow = rarity_col.alpha( static_cast< std::uint8_t >( ( 14.0f + 28.0f * hover_anim ) * fade_alpha ) );
				dl.rect_filled_gradient( card.x, card.y + card.h * 0.45f, card.w, card.h * 0.55f,
					glow.alpha( 0 ), glow.alpha( 0 ), glow, glow, xdraw::corner_radius::bottom( 8.0f ) );
			}

			// 4. Specular top rim
			dl.line( card.x + 8.0f, card.y + 0.5f, card.x + card.w - 8.0f, card.y + 0.5f,
				xdraw::color{ 255, 255, 255, static_cast< std::uint8_t >( ( 25.0f + 35.0f * hover_anim ) * fade_alpha ) }, 1.0f );

			// 5. Border
			auto bcol = is_equipped ? tokens::col_accent : ( kit ? rarity_col : xdraw::color{ 255, 255, 255, 25 } );
			bcol.a = static_cast< std::uint8_t >( ( is_equipped ? ( 160.0f + 80.0f * hover_anim ) : ( 30.0f + 65.0f * hover_anim ) ) * fade_alpha );
			dl.rect( card.x, card.y, card.w, card.h, bcol, xdraw::corner_radius{ 8.0f }, is_equipped ? 1.5f : 1.0f );

			if ( kit )
			{
				const auto img = econ.get_skin_image( kit->image_inventory );
				if ( img )
				{
					const auto target_h = image_h - 12.0f;
					const auto aspect = static_cast< float >( img->width ) / static_cast< float >( img->height );
					auto iw = target_h * aspect;
					auto ih = target_h;

					if ( iw > card.w - 14.0f )
					{
						iw = card.w - 14.0f;
						ih = iw / aspect;
					}

					const auto ix = std::floor( card.x + ( card.w - iw ) * 0.5f );
					const auto iy = std::floor( card.y + ( image_h - ih ) * 0.5f - hover_anim * 2.0f );
					const auto tint = xdraw::color{ 255, 255, 255, static_cast< std::uint8_t >( 255.0f * fade_alpha ) };

					dl.image( ix, iy, iw, ih, img->srv.Get( ), tint );
				}
				else
				{
					const auto cx = card.x + card.w * 0.5f;
					const auto cy = card.y + image_h * 0.5f;
					const auto icon_col = xui::lerp( tokens::col_text_dim, tokens::col_accent, hover_anim ).alpha( static_cast< std::uint8_t >( 200.0f * fade_alpha ) );
					dl.circle_filled( cx - 7.0f, cy + 6.0f, 3.5f, icon_col );
					dl.circle_filled( cx + 7.0f, cy + 3.0f, 3.5f, icon_col );
					dl.line( cx - 4.0f, cy + 6.0f, cx - 4.0f, cy - 7.0f, icon_col, 2.0f );
					dl.line( cx + 10.0f, cy + 3.0f, cx + 10.0f, cy - 10.0f, icon_col, 2.0f );
					dl.line( cx - 4.0f, cy - 7.0f, cx + 10.0f, cy - 10.0f, icon_col, 2.5f );
				}
			}
			else
			{
				const auto cx = card.x + card.w * 0.5f;
				const auto cy = card.y + image_h * 0.5f;
				const auto icon_col = xui::lerp( tokens::col_text_dim, tokens::col_accent, hover_anim ).alpha( static_cast< std::uint8_t >( 200.0f * fade_alpha ) );
				dl.circle( cx, cy, 12.0f, icon_col, 1.5f );
				dl.circle_filled( cx, cy, 3.0f, icon_col );
			}

			if ( kit )
			{
				auto bar = rarity_col;
				bar.a = static_cast< std::uint8_t >( ( 180.0f + 75.0f * hover_anim ) * fade_alpha );
				dl.rect_filled( card.x + 8.0f, card.bottom( ) - 3.5f, card.w - 16.0f, 2.5f, bar, xdraw::corner_radius{ 1.25f } );
			}

			const auto name_y = card.y + image_h + 4.0f;
			auto ncol = xui::lerp( tokens::col_text_dim, tokens::col_text, hover_anim );
			ncol.a = static_cast< std::uint8_t >( ncol.a * fade_alpha );

			const auto title = kit ? kit->localized_name : "Standard";
			const auto ntrunc = xui::truncate( title, card.w - 12.0f );
			const auto [ nw, nh ] = xdraw::measure_text( ntrunc );
			dl.text( std::floor( card.x + ( card.w - nw ) * 0.5f ), std::floor( name_y ), ntrunc, ncol );

			if ( is_equipped )
			{
				constexpr auto badge{ 14.0f };
				const auto bx = card.right( ) - badge - 4.0f;
				const auto by = card.y + 4.0f;

				auto badge_bg = tokens::col_accent;
				badge_bg.a = static_cast< std::uint8_t >( badge_bg.a * fade_alpha );
				dl.rect_filled( bx, by, badge, badge, badge_bg, xdraw::corner_radius{ badge * 0.5f } );

				const auto cx = bx + badge * 0.5f;
				const auto cy = by + badge * 0.5f;
				const auto check = xdraw::color{ tokens::col_dark.r, tokens::col_dark.g, tokens::col_dark.b, static_cast< std::uint8_t >( 255.0f * fade_alpha ) };

				const std::array<float, 6> pts
				{
					cx - badge * 0.20f, cy,
					cx - badge * 0.05f, cy + badge * 0.18f,
					cx + badge * 0.25f, cy - badge * 0.18f
				};

				dl.polyline( pts, check, false, 1.5f );
			}

			if ( hovered && input.mouse_clicked )
			{
				if ( !kit || is_equipped )
				{
					settings::g_changer.music.id = 0;
					hooks::cheat::trigger_lobby_music( 0 );
				}
				else
				{
					settings::g_changer.music.id = kit->id;
					hooks::cheat::trigger_lobby_music( static_cast< std::uint16_t >( kit->id ) );
				}
				notify_skin_changed( );
			}
		}

		static void integer_input( const char* label, int& value, int minimum, int maximum )
		{
			// xui text-input state must outlive a frame and be unique per item/field.
			static std::unordered_map<std::uintptr_t, std::string> buffers;
			const auto id = xui::make_id( label );
			auto& buffer = buffers[id];
			if ( xui::ctx( ).active_text_input != id ) buffer = std::to_string( value );
			xui::text_input( label, buffer, 10, "integer" );
			int parsed{};
			const auto result = std::from_chars( buffer.data( ), buffer.data( ) + buffer.size( ), parsed );
			const auto valid = result.ec == std::errc{} && result.ptr == buffer.data( ) + buffer.size( ) && parsed >= minimum && parsed <= maximum;
			if ( !valid )
				xui::text( std::format( "Enter {} to {}. Previous value kept.", minimum, maximum ), {235, 91, 105} );
			else if ( value != parsed )
			{
				value = parsed;
				notify_skin_changed( );
			}
		}

		static void draw_skin_editor( const features::changer::econ_item_system::item_def* weapon )
		{
			if ( !weapon ) return;
			auto& econ = features::changer::g_econ_item_system;
			const auto it = skin_map( ).find( weapon->def_index );
			if ( it == skin_map( ).end( ) )
			{
				xui::push_id( static_cast<std::uintptr_t>( weapon->def_index ) );
				if ( xui::button( "Configure Chams for this Weapon", 230.0f ) )
				{
					skins_ui.active_tab = browser_tab::chams;
				}
				xui::pop_id( );
				return;
			}

			auto& skin = it->second;
			const auto pk = econ.find_paint_kit( skin.paint_kit_id );
			const auto [low, high] = pk ? skin_options::wear_limits( pk->wear_min, pk->wear_max ) : std::pair{0.0f, 1.0f};
			skin.wear = skin_options::clamp_wear( skin.wear, low, high );
			const auto tier = skin_options::wear_tier( skin.wear );
			xui::push_id( static_cast<std::uintptr_t>( weapon->def_index ) );
			xui::section_header( "SKIN OPTIONS" );
			const auto full_title = pk ? std::format( "{} | {}", weapon->localized_name, pk->localized_name ) : weapon->localized_name;
			xui::text( rendering::theme::fit_text( full_title, xui::layout::item_width( ) ), tokens::col_text );
			xui::layout::spacing( 4.0f );

			const auto width = ( xui::layout::item_width( ) - xui::ctx( ).style.item_spacing_x * 4.0f ) / 5.0f;
			for ( int index = 0; index < 5; ++index )
			{
				if ( index ) xui::layout::same_line( );
				const auto preset = skin_options::wear_preset( index, low, high );
				const auto label = std::format( "{}{}", skin_options::wear_short[index], index == tier ? " *" : "" );
				xui::push_style_color( xui::style_col::text, preset ? tokens::col_text : tokens::col_text_dim.alpha( 95 ) );
				if ( xui::button( label, width ) && preset ) { skin.wear = *preset; notify_skin_changed( ); }
				xui::pop_style_color( );
			}
			xui::layout::new_line( );
			xui::layout::spacing( 4.0f );

			if ( low < high )
			{
				if ( xui::slider_float( "Wear", skin.wear, low, high, "%.6f" ) ) notify_skin_changed( );
			}
			else xui::text( std::format( "Fixed wear: {:.6f}", low ), tokens::col_text_dim );

			xui::layout::spacing( 4.0f );
			if ( xui::slider_int( "Pattern seed", skin.seed, 0, 1000, "%d" ) ) notify_skin_changed( );

			const auto supports_stattrak = weapon->category == features::changer::econ_item_system::item_category::gun ||
				weapon->category == features::changer::econ_item_system::item_category::knife;
			if ( supports_stattrak )
			{
				xui::layout::spacing( 4.0f );
				if ( xui::button( skin.stattrak ? "StatTrak: ON" : "StatTrak: OFF", 155.0f ) ) { skin.stattrak = !skin.stattrak; notify_skin_changed( ); }
				if ( skin.stattrak )
				{
					xui::layout::spacing( 4.0f );
					xui::text( "StatTrak count", tokens::col_text_dim );
					integer_input( "##stattrak_count", skin.stattrak_count, 0, std::numeric_limits<int>::max( ) );
				}
			}
			else
			{
				skin.stattrak = false;
			}

			xui::layout::spacing( 4.0f );
			xui::text( "Name tag", tokens::col_text_dim );
			static std::unordered_map<std::uintptr_t, std::string> name_tag_buffers;
			const auto name_tag_id = xui::make_id( "Name tag" );
			auto& name_tag_buffer = name_tag_buffers[ name_tag_id ];
			if ( xui::ctx( ).active_text_input != name_tag_id ) name_tag_buffer = skin.name_tag;
			if ( xui::text_input( "##name_tag_input", name_tag_buffer, 20, "Optional name tag" ) )
			{
				auto normalized = skin_options::normalize_name_tag( name_tag_buffer );
				if ( normalized != skin.name_tag )
				{
					skin.name_tag = std::move( normalized );
					notify_skin_changed( );
				}
			}

			xui::layout::spacing( 8.0f );
			if ( xui::button( "Configure Chams for this Weapon", 230.0f ) )
			{
				skins_ui.active_tab = browser_tab::chams;
			}

			xui::pop_id( );
		}

		static void draw_weapon_chams_editor( const features::changer::econ_item_system::item_def* weapon )
		{
			if ( !weapon ) return;
			auto& esp = settings::g_esp;
			const auto def_idx = weapon->def_index;
			const bool is_glove = ( weapon->category == features::changer::econ_item_system::item_category::glove );
			const auto chams_idx = chams_weapons::index( def_idx );

			xui::push_id( static_cast<std::uintptr_t>( def_idx ) ^ 0xccccull );

			if ( is_glove )
			{
				detail::draw_chams_config( "arms chams", "vm_arms_skins", esp.m_viewmodel.arms );
			}
			else if ( chams_idx >= 0 )
			{
				auto& entry = esp.m_viewmodel.individual.weapons[ chams_idx ];

				// If not yet customized, initialize values from global default
				if ( !entry.override_default.value )
				{
					entry.cfg.copy_values_from( esp.m_viewmodel.weapon );
					entry.cfg.enabled.value = false;
					entry.override_default.value = true;
				}

				detail::draw_chams_config( "weapon chams", "vm_indiv_skins", entry.cfg, true, false );
			}
			else
			{
				detail::draw_chams_config( "default weapon chams", "vm_def_skins_gen", esp.m_viewmodel.weapon );
			}

			xui::pop_id( );
		}

		static inline void draw_skin_tile( const xui::rect& card, const features::changer::econ_item_system::paint_kit* pk, const features::changer::econ_item_system::item_def* weapon, int current_kit_id, float fade_alpha )
		{
			auto& econ = features::changer::g_econ_item_system;
			auto& dl = xui::draw::current( );
			const auto& input = xui::ctx( ).input;

			const auto rarity = ( weapon && pk->id != 0 ) ? econ.combined_rarity( weapon->def_index, pk->id ) : pk->rarity;
			const auto rarity_col = k_rarity_colors[ std::clamp( static_cast<int>(rarity), 0, 7 ) ];

			const auto image_h = std::floor( card.h * k_image_h_ratio );

			const bool is_vanilla = ( pk->id == 0 );
			const bool is_equipped = is_vanilla ? ( current_kit_id <= 0 ) : ( pk->id == current_kit_id );
			const auto hovered = skin_workspace::hovered(card);
			const auto hover_anim = xui::anim::lerp( xui::fnv1a( "scard" ) + static_cast< std::uintptr_t >( pk->id ), hovered ? 1.0f : 0.0f, 14.0f );

			// Ambient drop shadow
			dl.rect_filled( card.x - 3.0f, card.y - 1.5f, card.w + 6.0f, card.h + 5.0f,
				xdraw::color{ 0, 0, 0, static_cast< std::uint8_t >( 24.0f * fade_alpha ) }, xdraw::corner_radius{ 10.0f } );
			dl.rect_filled( card.x, card.y + 1.5f, card.w, card.h,
				xdraw::color{ 0, 0, 0, static_cast< std::uint8_t >( 45.0f * fade_alpha ) }, xdraw::corner_radius{ 8.0f } );

			// 1. Frosted glass blur background
			dl.rect_filled_blurred( card.x, card.y, card.w, card.h, xdraw::corner_radius{ 8.0f },
				xdraw::color{ 160, 170, 190, static_cast< std::uint8_t >( 55.0f * fade_alpha ) } );

			// 2. Translucent frosted body with subtle dark tone
			const auto card_top = xdraw::color{ 22, 25, 34, static_cast< std::uint8_t >( ( 140.0f + 25.0f * hover_anim ) * fade_alpha ) };
			const auto card_bot = xdraw::color{ 14, 16, 22, static_cast< std::uint8_t >( ( 165.0f + 25.0f * hover_anim ) * fade_alpha ) };
			dl.rect_filled_gradient( card.x, card.y, card.w, card.h, card_top, card_top, card_bot, card_bot, xdraw::corner_radius{ 8.0f } );

			// 3. Ambient rarity glow
			if ( !is_vanilla )
			{
				const auto glow = rarity_col.alpha( static_cast< std::uint8_t >( ( 14.0f + 28.0f * hover_anim ) * fade_alpha ) );
				dl.rect_filled_gradient( card.x, card.y + card.h * 0.45f, card.w, card.h * 0.55f,
					glow.alpha( 0 ), glow.alpha( 0 ), glow, glow, xdraw::corner_radius::bottom( 8.0f ) );
			}

			// 4. Specular top rim
			dl.line( card.x + 8.0f, card.y + 0.5f, card.x + card.w - 8.0f, card.y + 0.5f,
				xdraw::color{ 255, 255, 255, static_cast< std::uint8_t >( ( 25.0f + 35.0f * hover_anim ) * fade_alpha ) }, 1.0f );

			// 5. Border
			auto bcol = is_equipped ? tokens::col_accent : ( is_vanilla ? xdraw::color{ 255, 255, 255, 25 } : rarity_col );
			bcol.a = static_cast< std::uint8_t >( ( is_equipped ? ( 160.0f + 80.0f * hover_anim ) : ( 30.0f + 65.0f * hover_anim ) ) * fade_alpha );
			dl.rect( card.x, card.y, card.w, card.h, bcol, xdraw::corner_radius{ 8.0f }, is_equipped ? 1.5f : 1.0f );

			const auto img = ( weapon && pk->id != 0 ) ? econ.get_skin_image( weapon->def_index, pk->id ) : ( weapon ? ( !weapon->image_inventory.empty( ) ? econ.get_skin_image( weapon->image_inventory ) : econ.get_skin_image( weapon->def_index, 0 ) ) : nullptr );
			if ( img )
			{
				const auto target_h = image_h - 12.0f;
				const auto aspect = static_cast< float >( img->width ) / static_cast< float >( img->height );
				auto iw = target_h * aspect;
				auto ih = target_h;

				if ( iw > card.w - 14.0f )
				{
					iw = card.w - 14.0f;
					ih = iw / aspect;
				}

				const auto ix = std::floor( card.x + ( card.w - iw ) * 0.5f );
				const auto iy = std::floor( card.y + ( image_h - ih ) * 0.5f - hover_anim * 2.0f );
				const auto tint = xdraw::color{ 255, 255, 255, static_cast< std::uint8_t >( 255.0f * fade_alpha ) };

				dl.image( ix, iy, iw, ih, img->srv.Get( ), tint );
			}
			else
			{
				auto ph_col = xui::darken( tokens::col_card, 0.7f );
				ph_col.a = static_cast< std::uint8_t >( 100.0f * fade_alpha );
				dl.rect_filled( card.x + 6.0f, card.y + 6.0f, card.w - 12.0f, image_h - 12.0f, ph_col, xdraw::corner_radius{ 4.0f } );
			}

			if ( !is_vanilla )
			{
				auto bar = rarity_col;
				bar.a = static_cast< std::uint8_t >( ( 180.0f + 75.0f * hover_anim ) * fade_alpha );
				dl.rect_filled( card.x + 8.0f, card.bottom( ) - 3.5f, card.w - 16.0f, 2.5f, bar, xdraw::corner_radius{ 1.25f } );
			}

			auto name_col = xui::lerp( tokens::col_text_dim, tokens::col_text, hover_anim );
			name_col.a = static_cast< std::uint8_t >( name_col.a * fade_alpha );

			const auto name_trunc = xui::truncate( pk->localized_name, card.w - 12.0f );
			const auto [nw, nh] = xdraw::measure_text( name_trunc );
			const auto name_y = card.y + image_h + k_rarity_bar_h + 4.0f;
			dl.text( std::floor( card.x + ( card.w - nw ) * 0.5f ), std::floor( name_y ), name_trunc, name_col );

			if ( is_equipped )
			{
				constexpr auto badge{ 14.0f };
				const auto bx = card.right( ) - badge - 4.0f;
				const auto by = card.y + 4.0f;

				auto badge_bg = tokens::col_accent;
				badge_bg.a = static_cast< std::uint8_t >( badge_bg.a * fade_alpha );
				dl.rect_filled( bx, by, badge, badge, badge_bg, xdraw::corner_radius{ badge * 0.5f } );

				const auto cx = bx + badge * 0.5f;
				const auto cy = by + badge * 0.5f;
				const auto check = xdraw::color{ tokens::col_dark.r, tokens::col_dark.g, tokens::col_dark.b, static_cast< std::uint8_t >( 255.0f * fade_alpha ) };

				const std::array<float, 6> pts
				{
					cx - badge * 0.20f, cy,
					cx - badge * 0.05f, cy + badge * 0.18f,
					cx + badge * 0.25f, cy - badge * 0.18f
				};

				dl.polyline( pts, check, false, 1.5f );
			}

			if ( hovered && input.mouse_clicked )
			{
				skins_ui.details_open = true;
				skins_ui.active_tab = browser_tab::skins;
				if ( is_equipped )
				{
					const auto browsing_def = econ.find_def( skins_ui.browsing_def );
					if ( !browsing_def || browsing_def->category != features::changer::econ_item_system::item_category::agent )
					{
						return;
					}

					skin_map( ).erase( skins_ui.browsing_def );
					notify_skin_changed( );
					request_page( skins_page::grid );
					return;
				}
				else
				{
					const auto browsing_def = econ.find_def( skins_ui.browsing_def );
					if ( pk->id == 0 )
					{
						if ( browsing_def && browsing_def->category == features::changer::econ_item_system::item_category::gun )
						{
							skin_map( ).erase( skins_ui.browsing_def );
						}
						else if ( browsing_def && browsing_def->category == features::changer::econ_item_system::item_category::glove )
						{
							for ( const auto* g : econ.gloves( ) )
							{
								skin_map( ).erase( g->def_index );
							}
						}
						else
						{
							settings::changer::applied_skin selected{};
							if ( const auto previous = skin_map( ).find( skins_ui.browsing_def ); previous != skin_map( ).end( ) )
								selected = previous->second;

							if ( browsing_def && browsing_def->category == features::changer::econ_item_system::item_category::knife )
							{
								for ( const auto* k : econ.knives( ) )
								{
									skin_map( ).erase( k->def_index );
								}
							}

							selected.paint_kit_id = 0;
							skin_map( )[ skins_ui.browsing_def ] = selected;
						}
					}
					else
					{
						settings::changer::applied_skin selected{};
						if ( const auto previous = skin_map( ).find( skins_ui.browsing_def ); previous != skin_map( ).end( ) )
							selected = previous->second;
						auto target_def = skins_ui.browsing_def;
						if ( browsing_def )
						{
							if ( browsing_def->category == features::changer::econ_item_system::item_category::knife )
							{
								for ( const auto* k : econ.knives( ) )
								{
									skin_map( ).erase( k->def_index );
								}
							}
							else if ( browsing_def->category == features::changer::econ_item_system::item_category::glove )
							{
								if ( pk->id != 0 )
								{
									for ( const auto& s : econ.skins( ) )
									{
										if ( s.paint_kit_id == pk->id )
										{
											if ( const auto* parent_def = econ.find_def( s.def_index ); parent_def && parent_def->category == features::changer::econ_item_system::item_category::glove )
											{
												target_def = s.def_index;
												break;
											}
										}
									}
								}
								for ( const auto* g : econ.gloves( ) )
								{
									skin_map( ).erase( g->def_index );
								}
							}
						}

						selected.paint_kit_id = pk->id;
						const auto [low, high] = skin_options::wear_limits( pk->wear_min, pk->wear_max );
						selected.wear = skin_options::clamp_wear( selected.wear, low, high );
						if ( browsing_def && browsing_def->category == features::changer::econ_item_system::item_category::glove ) selected.stattrak = false;
						skins_ui.browsing_def = target_def;
						skin_workspace::active_browsing_weapon = target_def;
						skin_workspace::focused_weapon[ skin_workspace::team == 3 ? 0 : 1 ] = target_def;
						skin_map( )[ target_def ] = selected;
					}
					notify_skin_changed( );
				}

				if ( auto win = xui::layout::current_window( ) )
				{
					auto& scroll = xui::ctx( ).child_scroll_cache[win->group_id];
					scroll.scroll = 0.0f;
					scroll.scroll_target = 0.0f;
				}
			}
		}

		static void draw_inventory_preview( const xui::rect& area, int category )
		{
			auto& dl = xui::draw::current( );
			auto& econ = features::changer::g_econ_item_system;
			const auto rx = std::round( area.x );
			const auto ry = std::round( area.y );
			const auto rw = static_cast<float>( std::max( 64, static_cast<int>( std::round( area.w ) ) ) );
			const auto rh = static_cast<float>( std::max( 64, static_cast<int>( std::round( area.h ) ) ) );
			dl.rect( rx, ry, rw, rh, tokens::col_border.alpha( 190 ), 1.0f );

			const xui::rect ct_button{ rx + rw - 130.0f, ry + 12.0f, 48.0f, 26.0f };
			const xui::rect t_button{ rx + rw - 72.0f, ry + 12.0f, 48.0f, 26.0f };
			if ( skin_workspace::button( ct_button, "CT", skin_workspace::team == 3 ) )
				skin_workspace::team = 3;
			if ( skin_workspace::button( t_button, "T", skin_workspace::team == 2 ) )
				skin_workspace::team = 2;

			systems::model_preview::request preview{};
			preview.team = skin_workspace::team;
			preview.visible = true;
			preview.width = static_cast<int>( rw );
			preview.height = static_cast<int>( rh );
			preview.x = rx;
			preview.y = ry;
			const auto [screen_w, screen_h] = xdraw::viewport_size( );
			preview.screen_width = static_cast<int>( screen_w );
			preview.screen_height = static_cast<int>( screen_h );
			preview.background_rgb = ( static_cast<std::uint32_t>(tokens::col_card.r) << 16 ) |
				( static_cast<std::uint32_t>(tokens::col_card.g) << 8 ) |
				static_cast<std::uint32_t>(tokens::col_card.b);

			if ( category == 4 )
			{
				preview.type = systems::model_preview::kind::music;
				preview.music_kit = settings::g_changer.music.id;
			}
			else if ( category == 3 )
			{
				preview.type = systems::model_preview::kind::agent;
				const auto& agents = settings::g_changer.agents;
				const auto& custom_agents = settings::g_changer.custom_agents;
				const auto custom_index = skin_workspace::team == 3 ? custom_agents.selected_ct : custom_agents.selected_t;
				preview.def_index = skin_workspace::team == 3 ? agents.ct_def : agents.t_def;
				if ( preview.def_index <= 0 )
				{
					for ( const auto* candidate : econ.agents( ) )
					{
						if ( candidate && ( candidate->team( ) == 0 || candidate->team( ) == skin_workspace::team ) )
						{
							preview.def_index = candidate->def_index;
							break;
						}
					}
					if ( preview.def_index <= 0 )
					{
						preview.def_index = ( skin_workspace::team == 3 ) ? 5105 : 500;
					}
				}
				if ( custom_index >= 0 && custom_index < static_cast<int>( custom_agents.entries.size( ) ) )
				{
					const auto& selected = custom_agents.entries[ custom_index ];
					if ( !selected.model_path.empty( ) )
					{
						preview.model_path = selected.model_path;
					}
				}
			}
			else
			{
				preview.type = category == 1 ? systems::model_preview::kind::knife :
					category == 2 ? systems::model_preview::kind::gloves : systems::model_preview::kind::weapon;

				const auto team_index = skin_workspace::team == 3 ? 0 : 1;
				const auto desired_category = category == 1 ? features::changer::econ_item_system::item_category::knife :
					category == 2 ? features::changer::econ_item_system::item_category::glove : features::changer::econ_item_system::item_category::gun;
				const auto focused = skin_workspace::focused_weapon[ team_index ];
				const auto selected_def = skins_ui.current == skins_page::browser && skins_ui.browsing_def != 0
					? skins_ui.browsing_def : focused;
				const auto* weapon = econ.find_def( selected_def );
				if ( !weapon || weapon->category != desired_category || ( weapon->team( ) != 0 && weapon->team( ) != skin_workspace::team ) )
				{
					weapon = nullptr;
					for ( const auto* candidate : skin_workspace::items.weapons( std::clamp( category, 0, 2 ) ) )
					{
						if ( candidate && candidate->category == desired_category &&
							( candidate->team( ) == 0 || candidate->team( ) == skin_workspace::team ) )
						{
							weapon = candidate;
							break;
						}
					}
				}

				if ( weapon )
				{
					preview.def_index = weapon->def_index;
					const auto& applied = settings::g_changer.skins.for_team( skin_workspace::team );
					if ( const auto it = applied.find( weapon->def_index ); it != applied.end( ) )
					{
						preview.paint_kit = it->second.paint_kit_id;
						preview.wear = it->second.wear;
						preview.seed = it->second.seed;
						preview.stattrak = it->second.stattrak;
						preview.stattrak_count = it->second.stattrak_count;
						preview.name_tag = it->second.name_tag;
					}

					if ( category == 2 && preview.paint_kit != 0 && ( preview.def_index == 5028 || preview.def_index == 5029 ) )
					{
						for ( const auto& s : econ.skins( ) )
						{
							if ( s.paint_kit_id == preview.paint_kit )
							{
								if ( const auto* parent_def = econ.find_def( s.def_index ); parent_def && parent_def->category == features::changer::econ_item_system::item_category::glove )
								{
									preview.def_index = s.def_index;
									break;
								}
							}
						}
					}
				}
			}

			static std::string rotation_asset;
			static float preview_yaw{};
			static float preview_pitch{};
			static bool rotating{};
			const auto asset_key = std::format( "{}|{}|{}|{}", static_cast<int>( preview.type ),
				preview.def_index, preview.team, preview.model_path );
			if ( rotation_asset != asset_key )
			{
				rotation_asset = asset_key;
				preview_yaw = 0.0f;
				preview_pitch = 0.0f;
				rotating = false;
			}
			const auto& input = xui::ctx( ).input;
			const bool over_team_button = ct_button.contains( input.mouse_x, input.mouse_y ) ||
				t_button.contains( input.mouse_x, input.mouse_y );
			if ( !input.rmb_down ) rotating = false;
			if ( input.rmb_clicked && area.contains( input.mouse_x, input.mouse_y ) && !over_team_button )
				rotating = true;
			if ( rotating && input.rmb_down )
			{
				preview_yaw = std::remainder( preview_yaw + input.mouse_delta_x( ) * 0.5f, 360.0f );
				preview_pitch = std::clamp( preview_pitch + input.mouse_delta_y( ) * 0.35f, -35.0f, 35.0f );
			}
			preview.yaw = preview_yaw;
			preview.pitch = preview_pitch;

			systems::g_model_preview.submit( std::move( preview ) );
		}

		static void draw_skin_detail_panes( float root_x, float root_y, float content_x, float list_y,
			float content_w, float list_h, float fade_alpha, const xui::style& style )
		{
			auto& econ = features::changer::g_econ_item_system;
			const auto& input = xui::ctx( ).input;
			const auto split_gap = 12.0f;
			const auto pane_w = std::floor( ( content_w - split_gap ) * 0.5f );
			const auto settings_w = std::max( 80.0f, content_w - pane_w - split_gap );
			const auto pane_h = std::max( 70.0f, list_h + style.window_pad_y );
			const auto weapon = econ.find_def( skins_ui.browsing_def );
			const auto& kits = skin_workspace::items.paints( skins_ui.browsing_def, skins_ui.search_buf );
			const auto applied_it = skin_map( ).find( skins_ui.browsing_def );
			const auto current_kit = applied_it != skin_map( ).end( ) ? applied_it->second.paint_kit_id : -1;

			// Keep the back/search row fixed. Only the paint catalogue below it scrolls.
			xui::layout::set_cursor( content_x - root_x, list_y - root_y );
			if ( xui::begin_child( "##skin_paint_panel", pane_w, pane_h, false ) )
			{
				const auto panel = xui::layout::current_window( )->bounds;
				const auto back_w = 82.0f;
				const auto bar_h = 28.0f;
				const auto bar_x = panel.x + style.window_pad_x;
				const auto bar_y = panel.y + style.window_pad_y;
				if ( skin_workspace::button( { bar_x, bar_y, back_w, bar_h }, "<  Back", false ) )
				{
					request_page( skins_page::grid );
					skins_ui.search_buf.clear( );
				}

				const auto search_x = bar_x + back_w + 8.0f;
				xui::layout::set_cursor( search_x - panel.x, bar_y - panel.y );
				xui::text_input( "##skin_search", skins_ui.search_buf, 64, "Search finishes" );

				const auto list_top = bar_y + bar_h + 10.0f;
				const auto list_w = panel.w - style.window_pad_x * 2.0f;
				const auto list_bottom = panel.bottom( ) - style.window_pad_y;
				const auto catalog_h = std::max( 48.0f, list_bottom - list_top );
				xui::layout::set_cursor( style.window_pad_x, list_top - panel.y );
				if ( xui::begin_child( "##skin_paint_catalogue", list_w, catalog_h, true, false ) )
				{
					auto* catalog = xui::layout::current_window( );
					const auto inset = 6.0f;
					const auto inner_w = std::max( 1.0f, catalog->bounds.w - inset * 2.0f );
					const auto columns = std::clamp( static_cast<int>( ( inner_w + detail::k_card_gap ) / 160.0f ), 1, 3 );
					const auto total_gap = ( columns - 1 ) * detail::k_card_gap;
					const auto card_w = std::floor( ( inner_w - total_gap ) / static_cast<float>( columns ) );
					const auto card_h = std::floor( card_w * ( detail::k_card_h_ref / detail::k_card_w_ref ) );
					const auto rows = ( static_cast<int>( kits.size( ) ) + columns - 1 ) / columns;
					const auto grid_h = rows * card_h + ( rows > 0 ? ( rows - 1 ) * detail::k_card_gap : 0.0f );
					const auto base_x = catalog->bounds.x + inset;
					const auto base_y = catalog->bounds.y - catalog->scroll_y;
					xui::layout::set_cursor( inset, -catalog->scroll_y );
					xui::layout::item( inner_w, grid_h );
					if ( kits.empty( ) ) xui::text( "No matching skins", tokens::col_text_dim );

					for ( auto i = 0; i < static_cast<int>( kits.size( ) ); ++i )
					{
						const auto col = i % columns;
						const auto row = i / columns;
						const auto cx = std::floor( base_x + col * ( card_w + detail::k_card_gap ) );
						const auto cy = std::floor( base_y + row * ( card_h + detail::k_card_gap ) );
						if ( cy + card_h < catalog->bounds.y || cy > catalog->bounds.bottom( ) ) continue;
						detail::draw_skin_tile( { cx, cy, card_w, card_h }, kits[ i ], weapon, current_kit, fade_alpha );
					}
					catalog->content_h = std::max( 0.0f, grid_h - catalog->scroll_y );
					xui::end_child( );
				}
				xui::end_child( );
			}

			// The settings pane is a sibling of the paint catalogue, with its own
			// scroll state and hit rectangle.
			const auto settings_x = content_x + pane_w + split_gap;
			xui::layout::set_cursor( settings_x - root_x, list_y - root_y );
			if ( xui::begin_child( "##skin_settings", settings_w, pane_h, true ) )
			{
				auto* settings_window = xui::layout::current_window( );
				const auto settings_bounds = settings_window->bounds;
				const auto settings_inner_w = settings_bounds.w - style.window_pad_x * 2.0f;
				xui::section_header( "ITEM SETTINGS" );
				const auto tab_gap = xui::ctx( ).style.item_spacing_x;
				const auto tab_w = ( settings_inner_w - tab_gap ) * 0.5f;
				if ( xui::button( "Skin", tab_w ) ) skins_ui.active_tab = browser_tab::skins;
				xui::layout::same_line( );
				if ( xui::button( "Weapon chams", tab_w ) ) skins_ui.active_tab = browser_tab::chams;
				xui::layout::new_line( );
				xui::layout::spacing( 8.0f );

				auto& settings_input = xui::ctx( ).input;
				const auto saved_clicked = settings_input.mouse_clicked;
				const auto saved_double_clicked = settings_input.mouse_double_clicked;
				if ( !input.in_rect( settings_bounds ) )
				{
					settings_input.mouse_clicked = false;
					settings_input.mouse_double_clicked = false;
				}
				if ( skins_ui.active_tab == browser_tab::chams )
				{
					detail::draw_weapon_chams_editor( weapon );
				}
				else
					detail::draw_skin_editor( weapon );
				settings_input.mouse_clicked = saved_clicked;
				settings_input.mouse_double_clicked = saved_double_clicked;
				settings_window->content_h = ( xui::layout::get_cursor( ).second + settings_window->scroll_y ) + style.window_pad_y + 24.0f;
				xui::end_child( );
			}
		}

	} // namespace detail

	void menu::draw_skins( float group_w )
	{
		skin_workspace::hover = {};
		skin_workspace::dialog_busy = detail::s_dialog_active.load();
		skin_workspace::items.refresh();
		const int previous_team = skin_workspace::team;
		this->draw_skins_browser(group_w);
		const int removed = std::exchange(skin_workspace::pending_delete_agent, -1);
		auto& ca = settings::g_changer.custom_agents;
		if (removed >= 0 && removed < static_cast<int>(ca.entries.size()))
		{
			ca.entries.erase(ca.entries.begin() + removed);
			for (int* selected : {&ca.selected_ct, &ca.selected_t})
				if (*selected == removed) *selected = -1; else if (*selected > removed) --*selected;
			skin_workspace::hover = {};
		}
		if (previous_team != skin_workspace::team)
		{
			xui::overlays::close_all();
			detail::skins_ui = {};
			if (this->m_subtab == 3)
			{
				detail::skins_ui.current = detail::skins_ui.target = detail::skins_page::browser;
				detail::skins_ui.browsing_agent_team = skin_workspace::team;
			}
		}
	}

	void menu::draw_skins_browser( float group_w ) const
	{
		(void)group_w;
		detail::process_pending_agents( );

		static auto last_subtab{ -1 };
		if ( this->m_subtab != last_subtab )
		{
			last_subtab = this->m_subtab;
			detail::skins_ui.details_open = false;
			detail::skins_ui.browsing_def = 0;
			skin_workspace::active_browsing_weapon = 0;
			detail::skins_ui.search_buf.clear( );
			detail::skins_ui.fade = 1.0f;
			if ( this->m_subtab == 3 )
			{
				if ( skin_workspace::team != 2 && skin_workspace::team != 3 )
					skin_workspace::team = 3;
				detail::skins_ui.browsing_agent_team = skin_workspace::team;
				detail::skins_ui.current = detail::skins_ui.target = detail::skins_page::browser;
			}
			else
			{
				detail::skins_ui.browsing_agent_team = 0;
				detail::skins_ui.current = detail::skins_ui.target = detail::skins_page::grid;
			}
		}

		auto& econ = features::changer::g_econ_item_system;
		auto& dl = xui::draw::current( );
		const auto& s = xui::ctx( ).style;
		const auto& input = xui::ctx( ).input;

		const auto wx = this->m_x;
		const auto wy = this->m_y;
		constexpr float k_panel_gap = 12.0f;
		const auto content_x = this->m_body_x;
		const auto body_y = this->m_body_y;
		const auto content_w = this->m_body_w;
		const auto body_h = this->m_body_h;
		if ( this->m_subtab == 3 )
		{
			if ( skin_workspace::team != 2 && skin_workspace::team != 3 )
				skin_workspace::team = 3;
			if ( detail::skins_ui.browsing_agent_team == 0 || detail::skins_ui.browsing_def != 0 || detail::skins_ui.details_open || detail::skins_ui.current != detail::skins_page::browser )
			{
				detail::skins_ui.browsing_def = 0;
				detail::skins_ui.details_open = false;
				skin_workspace::active_browsing_weapon = 0;
				detail::skins_ui.current = detail::skins_ui.target = detail::skins_page::browser;
				detail::skins_ui.browsing_agent_team = skin_workspace::team;
			}
		}

		const auto dt = xdraw::delta_time( );
		const auto fade_target = ( detail::skins_ui.current == detail::skins_ui.target ) ? 1.0f : 0.0f;
		detail::skins_ui.fade += ( fade_target - detail::skins_ui.fade ) * std::min( 14.0f * dt, 1.0f );

		if ( detail::skins_ui.fade < 0.05f && detail::skins_ui.current != detail::skins_ui.target )
		{
			detail::skins_ui.current = detail::skins_ui.target;
			detail::skins_ui.fade = 0.0f;
		}

		const auto fade_alpha = xui::ease::out_cubic( std::clamp( detail::skins_ui.fade, 0.0f, 1.0f ) );
		const auto preview_h = std::clamp( body_h * 0.5f, 138.0f, 340.0f );
		const auto list_y = body_y + preview_h + k_panel_gap;
		const auto list_h = std::max( 80.0f, body_h - preview_h - k_panel_gap );
		if ( this->m_open )
			detail::draw_inventory_preview( { content_x, body_y, content_w, preview_h }, this->m_subtab );
		else
			systems::g_model_preview.hide( );

		xui::layout::set_cursor( content_x - wx, list_y - wy );

		if ( detail::skins_ui.current == detail::skins_page::grid )
		{
			if ( !xui::begin_child( "##skins_grid", content_w, list_h, true ) )
			{
				return;
			}

			const auto win = xui::layout::current_window( );
			const auto inner_w = win->bounds.w - s.window_pad_x * 2.0f;

			const auto total_gap = ( detail::k_columns - 1 ) * detail::k_card_gap;
			const auto card_w = std::floor( ( inner_w - total_gap ) / static_cast< float >( detail::k_columns ) );
			const auto card_h = std::floor( card_w * ( detail::k_card_h_ref / detail::k_card_w_ref ) );

			const auto grid_w = card_w * detail::k_columns + total_gap;
			const auto offset_x = std::max( 0.0f, ( inner_w - grid_w ) * 0.5f );

			if ( this->m_subtab == 3 )
			{
				const auto base_x = win->bounds.x + s.window_pad_x + offset_x;
				const auto base_y = win->bounds.y + s.window_pad_y - win->scroll_y;

				const auto ct_card = xui::rect{ base_x, base_y, card_w, card_h };
				const auto t_card = xui::rect{ base_x + card_w + detail::k_card_gap, base_y, card_w, card_h };

				detail::draw_agent_team_card( ct_card, 3, fade_alpha );
				detail::draw_agent_team_card( t_card, 2, fade_alpha );

				xui::layout::item( inner_w, card_h );
				xui::end_child( );
				return;
			}

			if ( this->m_subtab == 4 )
			{
				constexpr auto bar_h{ 26.0f };
				const auto bar_x = win->bounds.x + s.window_pad_x;
				const auto bar_y = win->bounds.y + s.window_pad_y - win->scroll_y;

				xui::layout::set_cursor( s.window_pad_x, s.window_pad_y - win->scroll_y );
				xui::text_input( "##music_search", detail::skins_ui.search_buf, 64, "search music kits..." );

				const auto grid_top_y = bar_y + bar_h + 12.0f;
				const auto base_x = win->bounds.x + s.window_pad_x + offset_x;

				const auto& kits = skin_workspace::items.music(detail::skins_ui.search_buf);

				const auto total_tiles = static_cast< int >( kits.size( ) ) + 1;
				const auto rows = ( total_tiles + detail::k_columns - 1 ) / detail::k_columns;
				const auto grid_h = rows * card_h + ( rows > 0 ? ( rows - 1 ) * detail::k_card_gap : 0.0f );

				xui::layout::set_cursor( s.window_pad_x, s.window_pad_y );
				xui::layout::item( inner_w, ( bar_h + 12.0f ) + grid_h );

				const auto current_music_id = settings::g_changer.music.id;

				for ( auto i = 0; i < total_tiles; ++i )
				{
					const auto col = i % detail::k_columns;
					const auto row = i / detail::k_columns;

					const auto cx = std::floor( base_x + col * ( card_w + detail::k_card_gap ) );
					const auto cy = std::floor( grid_top_y + row * ( card_h + detail::k_card_gap ) );

					if ( cy + card_h < win->bounds.y || cy > win->bounds.bottom( ) )
					{
						continue;
					}

					const auto card = xui::rect{ cx, cy, card_w, card_h };
					if ( i == 0 )
					{
						detail::draw_music_tile( card, nullptr, current_music_id == 0, fade_alpha );
					}
					else
					{
						const auto* mk = kits[ i - 1 ];
						detail::draw_music_tile( card, mk, current_music_id == mk->id, fade_alpha );
					}
				}

				win->content_h = ( s.window_pad_y + bar_h + 12.0f + grid_h ) - win->scroll_y;
				xui::end_child( );
				return;
			}

			std::vector<const features::changer::econ_item_system::item_def*> items;
			for (const auto* item : skin_workspace::items.weapons(this->m_subtab))
			{
				if (!item) continue;
				if (this->m_subtab == 0 && item->category != features::changer::econ_item_system::item_category::gun) continue;
				if (item->team() != 0 && item->team() != skin_workspace::team) continue;
				items.push_back(item);
			}

			const auto rows = ( static_cast< int >( items.size( ) ) + detail::k_columns - 1 ) / detail::k_columns;
			const auto total_h = rows * card_h + ( rows > 0 ? ( rows - 1 ) * detail::k_card_gap : 0.0f );

			const auto base_x = win->bounds.x + s.window_pad_x + offset_x;
			const auto base_y = win->bounds.y + s.window_pad_y - win->scroll_y;

			xui::layout::item( inner_w, total_h );

			for ( auto i = 0; i < static_cast< int >( items.size( ) ); ++i )
			{
				const auto col = i % detail::k_columns;
				const auto row = i / detail::k_columns;

				const auto cx = std::floor( base_x + col * ( card_w + detail::k_card_gap ) );
				const auto cy = std::floor( base_y + row * ( card_h + detail::k_card_gap ) );

				if ( cy + card_h < win->bounds.y || cy > win->bounds.bottom( ) )
				{
					continue;
				}

				const auto card = xui::rect{ cx, cy, card_w, card_h };
				detail::draw_weapon_card( card, items[ i ], fade_alpha );
			}

			xui::end_child( );
		}
		else
		{
			if ( detail::skins_ui.details_open )
			{
				detail::draw_skin_detail_panes( wx, wy, content_x, list_y, content_w, list_h, fade_alpha, s );
				return;
			}

			if ( !xui::begin_child( "##skins_browser", content_w, list_h, true ) )
			{
				return;
			}

			const auto win = xui::layout::current_window( );
			const auto inner_w = win->bounds.w - s.window_pad_x * 2.0f;

			const auto total_gap = ( detail::k_columns - 1 ) * detail::k_card_gap;
			const auto card_w = std::floor( ( inner_w - total_gap ) / static_cast< float >( detail::k_columns ) );
			const auto card_h = std::floor( card_w * ( detail::k_card_h_ref / detail::k_card_w_ref ) );

			const auto grid_w = card_w * detail::k_columns + total_gap;
			const auto offset_x = std::max( 0.0f, ( inner_w - grid_w ) * 0.5f );

			constexpr auto bar_h{ 26.0f };
			const auto bar_x = win->bounds.x + s.window_pad_x;
			const auto bar_y = win->bounds.y + s.window_pad_y - win->scroll_y;

			const auto back_w{ 88.0f };
			const auto back_rect = xui::rect{ bar_x, bar_y, back_w, bar_h };
			const auto back_hovered = skin_workspace::hovered(back_rect);
			const auto back_hover = xui::anim::lerp( xui::fnv1a( "skin_back" ), back_hovered ? 1.0f : 0.0f, 14.0f );

			if ( back_hovered && input.mouse_clicked )
			{
				if ( this->m_subtab == 3 )
				{
					const_cast<menu*>(this)->m_subtab = 0;
				}
				else
				{
					detail::request_page( detail::skins_page::grid );
				}
				detail::skins_ui.search_buf.clear( );
			}

			auto back_bg = xui::lerp( s.button_bg, s.button_hovered, back_hover );
			back_bg.a = static_cast< std::uint8_t >( back_bg.a * fade_alpha );
			dl.rect_filled( back_rect.x, back_rect.y, back_rect.w, back_rect.h, back_bg, xdraw::corner_radius{ s.button_rounding } );

			const auto [bw, bh] = xdraw::measure_text( "<  Weapons" );
			auto back_text = xui::lerp( s.text_dim, s.text, back_hover );
			back_text.a = static_cast< std::uint8_t >( back_text.a * fade_alpha );
			dl.text( back_rect.x + ( back_rect.w - bw ) * 0.5f, back_rect.y + ( back_rect.h - bh ) * 0.5f, "<  Weapons", back_text );

			float next_header_x = back_rect.right( ) + 6.0f;
			xui::layout::set_cursor( next_header_x - win->bounds.x, bar_y - win->bounds.y );
			xui::text_input( "##skin_search", detail::skins_ui.search_buf, 64, "search..." );

			auto grid_top_y = bar_y + bar_h + 12.0f;
			const auto base_x = win->bounds.x + s.window_pad_x + offset_x;

			if ( detail::skins_ui.browsing_agent_team != 0 )
			{
				const auto add_btn_w{ 95.0f };
				const auto add_btn_rect = xui::rect{ bar_x, grid_top_y, add_btn_w, bar_h };
				grid_top_y += bar_h + 12.0f;
				const auto add_btn_hovered = skin_workspace::hovered(add_btn_rect);
				const auto add_btn_hover = xui::anim::lerp( xui::fnv1a( "add_custom_btn" ), add_btn_hovered ? 1.0f : 0.0f, 14.0f );

				if ( add_btn_hovered && input.mouse_clicked )
				{
					detail::open_agent_file_dialog( detail::skins_ui.browsing_agent_team );
				}

				auto add_btn_bg = xui::lerp( tokens::col_accent, xui::lighten( tokens::col_accent, 1.2f ), add_btn_hover );
				add_btn_bg.a = static_cast< std::uint8_t >( ( 190.0f + 65.0f * add_btn_hover ) * fade_alpha );
				dl.rect_filled( add_btn_rect.x, add_btn_rect.y, add_btn_rect.w, add_btn_rect.h, add_btn_bg, xdraw::corner_radius{ s.button_rounding } );

				const auto add_btn_text = detail::s_dialog_active ? "... Loading" : "+ Custom";
				const auto [atw, ath] = xdraw::measure_text( add_btn_text );
				dl.text( add_btn_rect.x + ( add_btn_rect.w - atw ) * 0.5f, add_btn_rect.y + ( add_btn_rect.h - ath ) * 0.5f, add_btn_text, xdraw::color{ 255, 255, 255, static_cast< std::uint8_t >( 255.0f * fade_alpha ) } );

				std::string search_lower = detail::skins_ui.search_buf;
				for ( auto& c : search_lower )
				{
					c = static_cast< char >( std::tolower( c ) );
				}

				// Collect matching custom agents
				struct custom_item { int idx; const settings::changer::custom_agent_entry* entry; };
				std::vector<custom_item> custom_items;
				const auto& ca = settings::g_changer.custom_agents;
				for ( int i = 0; i < static_cast< int >( ca.entries.size( ) ); ++i )
				{
					const auto& e = ca.entries[ i ];
					if ( e.team != 0 && e.team != detail::skins_ui.browsing_agent_team )
						continue;

					if ( !search_lower.empty( ) )
					{
						std::string n = e.name;
						for ( auto& c : n ) c = static_cast< char >( std::tolower( c ) );
						if ( n.find( search_lower ) == std::string::npos && e.model_path.find( search_lower ) == std::string::npos )
							continue;
					}

					custom_items.push_back( { i, &e } );
				}

				const auto& agent_items = skin_workspace::items.agents( detail::skins_ui.browsing_agent_team, detail::skins_ui.search_buf );

				const auto total_count = static_cast< int >( custom_items.size( ) + agent_items.size( ) );
				const auto rows = ( total_count + detail::k_columns - 1 ) / detail::k_columns;
				const auto grid_h = rows * card_h + ( rows > 0 ? ( rows - 1 ) * detail::k_card_gap : 0.0f );

				xui::layout::set_cursor( s.window_pad_x, s.window_pad_y );
				xui::layout::item( inner_w, 2.0f * ( bar_h + 12.0f ) + grid_h );

				const auto& sel = settings::g_changer.agents;
				const auto current_agent = ( detail::skins_ui.browsing_agent_team == 3 ) ? sel.ct_def : sel.t_def;
				const auto current_custom = ( detail::skins_ui.browsing_agent_team == 3 ) ? ca.selected_ct : ca.selected_t;

				for ( auto i = 0; i < total_count; ++i )
				{
					const auto col = i % detail::k_columns;
					const auto row = i / detail::k_columns;

					const auto cx = std::floor( base_x + col * ( card_w + detail::k_card_gap ) );
					const auto cy = std::floor( grid_top_y + row * ( card_h + detail::k_card_gap ) );

					if ( cy + card_h < win->bounds.y || cy > win->bounds.bottom( ) )
					{
						continue;
					}

					const auto card = xui::rect{ cx, cy, card_w, card_h };
					if ( i < static_cast< int >( custom_items.size( ) ) )
					{
						const auto& ci = custom_items[ i ];
						detail::draw_custom_agent_tile( card, ci.idx, *ci.entry, ci.idx == current_custom, fade_alpha );
					}
					else
					{
						const auto a_idx = i - static_cast< int >( custom_items.size( ) );
						detail::draw_agent_tile( card, agent_items[ a_idx ], ( current_custom < 0 && agent_items[ a_idx ]->def_index == current_agent ), fade_alpha );
					}
				}

				win->content_h = grid_top_y - win->bounds.y + grid_h;
				xui::end_child( );
				return;
			}

			const auto weapon = econ.find_def( detail::skins_ui.browsing_def );
			const auto& kits = skin_workspace::items.paints( detail::skins_ui.browsing_def, detail::skins_ui.search_buf );
			const auto applied_it = detail::skin_map( ).find( detail::skins_ui.browsing_def );
			const auto current_kit = ( applied_it != detail::skin_map( ).end( ) ) ? applied_it->second.paint_kit_id : -1;

			if ( detail::skins_ui.details_open )
			{
				const auto parent_bounds = win->bounds;
				const auto child_y = grid_top_y - parent_bounds.y;
				const auto child_h = std::max( 70.0f, parent_bounds.h - child_y - s.window_pad_y );
				const auto split_gap = 10.0f;
				const auto paint_w = std::floor( ( inner_w - split_gap ) * 0.53f );
				const auto settings_w = std::max( 80.0f, inner_w - paint_w - split_gap );

				// The paint catalogue and editor scroll independently so the selected
				// item stays visible while tuning its finish.
				xui::layout::set_cursor( s.window_pad_x, child_y );
				if ( xui::begin_child( "##skin_paint_list", paint_w, child_h, true ) )
				{
					auto* paint_window = xui::layout::current_window( );
					const auto paint_inner_w = paint_window->bounds.w - s.window_pad_x * 2.0f;
					const auto columns = std::clamp( static_cast<int>( ( paint_inner_w + detail::k_card_gap ) / 160.0f ), 1, 5 );
					const auto paint_gap = ( columns - 1 ) * detail::k_card_gap;
					const auto paint_card_w = std::floor( ( paint_inner_w - paint_gap ) / static_cast<float>( columns ) );
					const auto paint_card_h = std::floor( paint_card_w * ( detail::k_card_h_ref / detail::k_card_w_ref ) );
					const auto paint_base_x = paint_window->bounds.x + s.window_pad_x;
					const auto paint_base_y = paint_window->bounds.y + s.window_pad_y - paint_window->scroll_y;
					const auto rows = ( static_cast<int>( kits.size( ) ) + columns - 1 ) / columns;
					const auto grid_h = rows * paint_card_h + ( rows > 0 ? ( rows - 1 ) * detail::k_card_gap : 0.0f );
					xui::layout::set_cursor( s.window_pad_x, s.window_pad_y - paint_window->scroll_y );
					xui::layout::item( paint_inner_w, grid_h );
					if ( kits.empty( ) ) xui::text( "No matching skins", tokens::col_text_dim );

					for ( auto i = 0; i < static_cast<int>( kits.size( ) ); ++i )
					{
						const auto col = i % columns;
						const auto row = i / columns;
						const auto cx = std::floor( paint_base_x + col * ( paint_card_w + detail::k_card_gap ) );
						const auto cy = std::floor( paint_base_y + row * ( paint_card_h + detail::k_card_gap ) );
						if ( cy + paint_card_h < paint_window->bounds.y || cy > paint_window->bounds.bottom( ) ) continue;
						detail::draw_skin_tile( { cx, cy, paint_card_w, paint_card_h }, kits[ i ], weapon, current_kit, fade_alpha );
					}
					paint_window->content_h = s.window_pad_y + grid_h;
					xui::end_child( );
				}

				if ( auto* parent = xui::layout::current_window( ) )
					xui::layout::set_cursor( s.window_pad_x + paint_w + split_gap, child_y );
				if ( xui::begin_child( "##skin_settings", settings_w, child_h, true ) )
				{
					auto* settings_window = xui::layout::current_window( );
					const auto settings_inner_w = settings_window->bounds.w - s.window_pad_x * 2.0f;
					xui::section_header( "ITEM SETTINGS" );
					const auto tab_gap = xui::ctx( ).style.item_spacing_x;
					const auto tab_w = ( settings_inner_w - tab_gap ) * 0.5f;
					if ( xui::button( "Skin", tab_w ) ) detail::skins_ui.active_tab = detail::browser_tab::skins;
					xui::layout::same_line( );
					if ( xui::button( "Weapon chams", tab_w ) ) detail::skins_ui.active_tab = detail::browser_tab::chams;
					xui::layout::new_line( );
					xui::layout::spacing( 8.0f );

					auto& settings_input = xui::ctx( ).input;
					const auto saved_clicked = settings_input.mouse_clicked;
					const auto saved_double_clicked = settings_input.mouse_double_clicked;
					if ( !input.in_rect( settings_window->bounds ) )
					{
						settings_input.mouse_clicked = false;
						settings_input.mouse_double_clicked = false;
					}
					if ( detail::skins_ui.active_tab == detail::browser_tab::chams )
					{
						detail::draw_weapon_chams_editor( weapon );
					}
					else
						detail::draw_skin_editor( weapon );
					settings_input.mouse_clicked = saved_clicked;
					settings_input.mouse_double_clicked = saved_double_clicked;
					settings_window->content_h = ( xui::layout::get_cursor( ).second + settings_window->scroll_y ) + s.window_pad_y + 24.0f;
					xui::end_child( );
				}

				if ( auto* parent = xui::layout::current_window( ) )
					parent->content_h = parent->bounds.h;
				xui::end_child( );
			}
			else
			{
				const auto rows = ( static_cast<int>( kits.size( ) ) + detail::k_columns - 1 ) / detail::k_columns;
				const auto grid_h = rows * card_h + ( rows > 0 ? ( rows - 1 ) * detail::k_card_gap : 0.0f );
				xui::layout::set_cursor( s.window_pad_x, grid_top_y - win->bounds.y );
				xui::layout::item( inner_w, grid_h );
				if ( kits.empty( ) ) xui::text( "No matching skins", tokens::col_text_dim );

				for ( auto i = 0; i < static_cast<int>( kits.size( ) ); ++i )
				{
					const auto col = i % detail::k_columns;
					const auto row = i / detail::k_columns;
					const auto cx = std::floor( base_x + col * ( card_w + detail::k_card_gap ) );
					const auto cy = std::floor( grid_top_y + row * ( card_h + detail::k_card_gap ) );
					if ( cy + card_h < win->bounds.y || cy > win->bounds.bottom( ) ) continue;
					detail::draw_skin_tile( { cx, cy, card_w, card_h }, kits[ i ], weapon, current_kit, fade_alpha );
				}

				win->content_h = grid_top_y - win->bounds.y + grid_h;
				xui::end_child( );
			}
		}
	}

} // namespace rendering
