#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <utilities/logging/logging.hpp>
#include <core/systems/systems.hpp>

#include <core/features/misc/misc.hpp>

namespace systems
{
    bool model_preview::initialize( )
    {
        m_initialized = true;
        m_current_texture = nullptr;
        m_panel_spawned = false;
        m_spawn_throttle = 0;
        m_root_panel = nullptr;
        logging::console::print( "[model_preview] initialized" );
        return true;
    }

    void model_preview::spawn_preview_panel( )
    {
        if ( m_panel_spawned )
            return;

        if ( !addresses::globals::panorama )
            return;

        auto* panorama = reinterpret_cast<features::misc::c_panorama_ui_engine*>( addresses::globals::panorama );
        if ( !panorama )
            return;

        auto* ui_engine = panorama->get_ui_engine( );
        if ( !ui_engine )
            return;

        features::misc::c_ui_panel* root_panel = nullptr;

        if ( addresses::globals::hud )
        {
            const auto hud = memory::safe_read<std::uintptr_t>( addresses::globals::hud ).value_or( 0 );
            if ( hud )
            {
                auto* p = memory::safe_read<features::misc::c_ui_panel*>( hud + 0x8 ).value_or( nullptr );
                if ( p )
                {
                    const auto vtable = memory::safe_read<std::uintptr_t>( reinterpret_cast<std::uintptr_t>( p ) ).value_or( 0 );
                    if ( vtable )
                        root_panel = p;
                }
            }
        }

        if ( !root_panel && ui_engine->m_panel_count > 0 && ui_engine->m_panels_array )
        {
            for ( int i = 0; i < std::min( ui_engine->m_panel_count, 128 ); ++i )
            {
                auto* p = ui_engine->m_panels_array[ i ].m_panel;
                if ( p )
                {
                    const auto vtable = memory::safe_read<std::uintptr_t>( reinterpret_cast<std::uintptr_t>( p ) ).value_or( 0 );
                    if ( vtable )
                    {
                        root_panel = p;
                        break;
                    }
                }
            }
        }

        if ( !root_panel )
            return;

        static constexpr const char* k_spawn_script = R"PANORAMA(
(function() {
    try {
        function getRoot() {
            var p = $.GetContextPanel();
            if (!p) return null;
            while (p && typeof p.GetParent === 'function' && p.GetParent()) {
                p = p.GetParent();
            }
            return p;
        }
        var root = getRoot();
        if (!root) return;
        var preview = root.FindChildTraverse("MintalyOffscreenPreview");
        if (!preview) {
            preview = $.CreatePanel("MapPlayerPreviewPanel", root, "MintalyOffscreenPreview", {
                map: "ui/buy_menu",
                camera: "cam_loadoutmenu_ct",
                "require-composition-layer": "true",
                "composition-layer-texture-name": "mintaly_preview_tex",
                playermodel: "characters/models/ctm_sas/ctm_sas.vmdl",
                playername: "vanity_character",
                animgraphcharactermode: "buy-menu",
                player: "true",
                mouse_rotate: "true"
            });
            if (preview) {
                preview.style.visibility = "visible";
                preview.style.opacity = "0.01";
                preview.style.width = "512px";
                preview.style.height = "512px";
                preview.style.position = "0px 0px 0px";
                preview.style.zIndex = "-9999";
            }
        }
        $.MintalySetPreview = function(model, defIndex) {
            try {
                var p = root.FindChildTraverse("MintalyOffscreenPreview");
                if (!p) return;
                if (model && model.length > 0 && typeof p.SetPlayerModel === 'function') {
                    p.SetPlayerModel(model);
                }
                if (defIndex && defIndex > 0 && typeof p.EquipPlayerWithItem === 'function') {
                    var itemId = (BigInt('0xF000000000000000') | BigInt(defIndex)).toString();
                    p.EquipPlayerWithItem(itemId);
                }
            } catch(e) {}
        };
    } catch(e) {}
})();
)PANORAMA";

        ui_engine->run_script( root_panel, k_spawn_script );
        m_root_panel = root_panel;
        m_panel_spawned = true;
        logging::console::print( "[model_preview] MapPlayerPreviewPanel spawned in Panorama\n" );
    }

    void model_preview::set_item( int def_index )
    {
        if ( !addresses::globals::panorama || !m_panel_spawned || !m_root_panel )
            return;

        auto* panorama = reinterpret_cast<features::misc::c_panorama_ui_engine*>( addresses::globals::panorama );
        if ( !panorama ) return;
        auto* ui_engine = panorama->get_ui_engine( );
        if ( !ui_engine ) return;

        const std::string script = std::format(
            "if (typeof $.MintalySetPreview === 'function') {{ $.MintalySetPreview('', {}); }}",
            def_index
        );
        ui_engine->run_script( reinterpret_cast<features::misc::c_ui_panel*>( m_root_panel ), script.c_str( ) );
    }

    void model_preview::set_agent( const std::string& model_path )
    {
        if ( !addresses::globals::panorama || !m_panel_spawned || !m_root_panel )
            return;

        auto* panorama = reinterpret_cast<features::misc::c_panorama_ui_engine*>( addresses::globals::panorama );
        if ( !panorama ) return;
        auto* ui_engine = panorama->get_ui_engine( );
        if ( !ui_engine ) return;

        const std::string script = std::format(
            "if (typeof $.MintalySetPreview === 'function') {{ $.MintalySetPreview('{}', 0); }}",
            model_path
        );
        ui_engine->run_script( reinterpret_cast<features::misc::c_ui_panel*>( m_root_panel ), script.c_str( ) );
    }

    void model_preview::update( )
    {
        if ( !m_initialized )
            return;

        if ( !m_current_texture && !m_panel_spawned )
        {
            ++m_spawn_throttle;
            if ( m_spawn_throttle % 20 == 0 )
            {
                spawn_preview_panel( );
            }
        }
    }

    bool model_preview::on_generate_primitives(
        std::uintptr_t owner_entity,
        std::uint32_t owner_hash,
        std::uintptr_t scene_object,
        std::uintptr_t primitive_buffer,
        void( __fastcall* original_fn )( std::uintptr_t, std::uintptr_t, std::uintptr_t, std::uintptr_t ),
        std::uintptr_t a1,
        std::uintptr_t scene_view )
    {
        if ( !m_initialized || !scene_object || !a1 )
            return false;

        const auto scene_layer = memory::safe_read<std::uintptr_t>( a1 + 0x10 ).value_or( 0 );
        const auto scene_layer_backup = scene_layer ? scene_layer : memory::safe_read<std::uintptr_t>( a1 + 0x98 ).value_or( 0 );
        if ( !scene_layer_backup )
            return false;

        // Check for offscreen DirectX 11 render target composition layer
        const auto tex_handle = memory::safe_read<std::uintptr_t>( scene_layer_backup + 0x07D8 ).value_or( 0 );
        const auto tex_handle_backup = tex_handle ? tex_handle : memory::safe_read<std::uintptr_t>( scene_layer_backup + 0x0818 ).value_or( 0 );
        if ( !tex_handle_backup )
            return false;

        const auto tex_dx11 = memory::safe_read<std::uintptr_t>( tex_handle_backup ).value_or( 0 );
        if ( !tex_dx11 )
            return false;

        const auto srv0 = memory::safe_read<ID3D11ShaderResourceView*>( tex_dx11 + 0x10 ).value_or( nullptr );
        const auto srv = srv0 ? srv0 : memory::safe_read<ID3D11ShaderResourceView*>( tex_dx11 + 0x18 ).value_or( nullptr );
        if ( srv )
        {
            const auto vtable = memory::safe_read<std::uintptr_t>( reinterpret_cast<std::uintptr_t>( srv ) ).value_or( 0 );
            if ( vtable && vtable > 0x10000 )
            {
                m_current_texture = reinterpret_cast<void*>( tex_dx11 );
            }
        }

        return false;
    }
}

