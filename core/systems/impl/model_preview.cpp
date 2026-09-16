#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <utilities/logging/logging.hpp>
#include <core/systems/systems.hpp>

namespace systems
{
    bool model_preview::initialize( )
    {
        m_initialized = true;
        m_current_texture = nullptr;
        logging::console::print( "[model_preview] initialized" );
        return true;
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
        if ( !m_initialized || !owner_entity || !scene_object )
            return false;

        if ( owner_hash != "C_CSGO_PreviewPlayer"_hash )
            return false;

        // Allow capturing any game preview agent (CT, T, custom agents, inspect models)

        const auto* data = reinterpret_cast<const c_generate_primitives_data*>( scene_object );
        if ( data && reinterpret_cast<std::uintptr_t>( data ) > 0x10000 )
        {
            const auto* scene_layer = data->m_scene_layer;
            if ( scene_layer && reinterpret_cast<std::uintptr_t>( scene_layer ) > 0x10000 )
            {
                const auto* tex_handle = scene_layer->m_texture_handle;
                if ( tex_handle && reinterpret_cast<std::uintptr_t>( tex_handle ) > 0x10000 )
                {
                    if ( tex_handle->m_texture && reinterpret_cast<std::uintptr_t>( tex_handle->m_texture ) > 0x10000 )
                    {
                        m_current_texture = tex_handle->m_texture;

                        static bool first_log = true;
                        if ( first_log )
                        {
                            logging::console::print( "[model_preview] captured preview texture!" );
                            first_log = false;
                        }
                    }
                }
            }
        }

        return false;
    }
}