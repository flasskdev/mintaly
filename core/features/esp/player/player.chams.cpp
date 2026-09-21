#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <utilities/logging/logging.hpp>
#include <core/systems/systems.hpp>
#include <core/settings.hpp>
#include <core/features/features.hpp>
#include <protection/game_addresses.hpp>
#include "../primitive_buffer.hpp"
#include "../weapon_definition.hpp"

namespace features::esp::player {

	static void mark_range_last_fast( const detail::primitive_output_buffer* buffer, int prev_count, int new_count ) noexcept
	{
		__try
		{
			for ( auto i = prev_count; i < new_count; ++i )
			{
				const auto prim = buffer->at_fast( i );
				if ( prim )
				{
					detail::mark_primitive_last_fast( prim );
				}
			}
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
		}
	}

	template< typename F >
	void chams::onshot::render_for_entity( std::uintptr_t entity, std::uintptr_t ragdoll_pawn, std::uintptr_t model_handle, std::uintptr_t primitive_buffer, void( __fastcall* original_fn )( std::uintptr_t, std::uintptr_t, std::uintptr_t, std::uintptr_t ), std::uintptr_t a1, std::uintptr_t scene_view, F&& apply_config_fn )
	{
		std::lock_guard lock( this->m_mtx );
		if ( this->m_entries.empty( ) )
			return;

		const auto global_vars = memory::read<std::uintptr_t>( addresses::globals::global_vars );
		const auto current_time = global_vars ? memory::read<float>( global_vars + 0x30 ) : 0.0f;
		const auto fade_time = settings::g_esp.m_player.m_chams.onshot_fade_time.value;
		if ( fade_time <= 0.0f )
			return;

		const auto& ocfg = settings::g_esp.m_player.m_chams.onshot;
		if ( !ocfg.enabled.value )
			return;

		for ( const auto& e : this->m_entries )
		{
			if ( !e.scene_object )
				continue;

			const bool matches = ( entity && e.pawn == entity ) ||
								 ( ragdoll_pawn && e.pawn == ragdoll_pawn ) ||
								 ( model_handle && e.model_handle == model_handle );
			if ( !matches )
				continue;

			const auto elapsed = current_time - e.spawn_time;
			if ( elapsed < 0.0f || elapsed >= fade_time )
				continue;

			const auto alpha = std::clamp( 1.0f - ( elapsed / fade_time ), 0.0f, 1.0f );
			if ( alpha <= 0.001f )
				continue;

			const auto before = detail::read_primitive_buffer( primitive_buffer );
			const auto prev_count = before ? before->count( ) : -1;

			apply_config_fn( ocfg, e.scene_object, true, alpha );

			const auto after = detail::read_primitive_buffer( primitive_buffer );
			const auto new_count = after ? after->count( ) : -1;
			if ( after && prev_count >= 0 && new_count > prev_count )
			{
				mark_range_last_fast( &*after, prev_count, new_count );
			}
		}
	}

	bool chams::on_generate_primitives( std::uintptr_t owner_entity, std::uint32_t owner_hash, std::uintptr_t scene_object, std::uintptr_t primitive_buffer, void( __fastcall* original_fn )( std::uintptr_t, std::uintptr_t, std::uintptr_t, std::uintptr_t ), std::uintptr_t a1, std::uintptr_t scene_view )
	{
		const auto is_player = owner_hash == "C_CSPlayerPawn"_hash;
		const auto is_ragdoll = owner_hash == "C_CSRagdoll"_hash;
		const auto is_arms = owner_hash == "C_CS2HudModelArms"_hash;
		const auto is_weapon = owner_hash == "C_CS2HudModelWeapon"_hash;

		const auto is_local_attachment = [ & ]( std::uintptr_t view_pawn ) -> bool
			{
				if ( !owner_entity || owner_entity < 0x10000 )
				{
					return false;
				}

				const auto game_scene_node = memory::safe_read<std::uintptr_t>( owner_entity + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) ).value_or( 0 );
				if ( !game_scene_node || game_scene_node < 0x10000 )
				{
					return false;
				}

				const auto parent_node = memory::safe_read<std::uintptr_t>( game_scene_node + SCHEMA( "CGameSceneNode", "m_pParent"_hash ) ).value_or( 0 );
				if ( !parent_node || parent_node < 0x10000 )
				{
					return false;
				}

				const auto parent_owner = memory::safe_read<std::uintptr_t>( parent_node + SCHEMA( "CGameSceneNode", "m_pOwner"_hash ) ).value_or( 0 );
				return parent_owner == view_pawn;
			};

		const auto apply_config = [ & ]( const settings::esp::chams_config& cfg, std::uintptr_t target_scene_obj, bool force_original = false, float alpha = 1.0f )
			{
				const auto fade = [alpha]( xdraw::color color ) {
					color.a = static_cast<std::uint8_t>( color.a * alpha );
					return color;
				};
				const bool overlay_is_outline = cfg.overlay.enabled.value && settings::esp::is_outline_material( cfg.overlay.material.value );
				const bool overlay_suppress_fill = overlay_is_outline && !cfg.overlay.filled.value;

				const bool primary_is_outline = cfg.primary.enabled.value && settings::esp::is_outline_material( cfg.primary.material.value );
				const bool primary_suppress_fill = primary_is_outline && !cfg.primary.filled.value;

				const bool secondary_is_outline = cfg.secondary.enabled.value && settings::esp::is_outline_material( cfg.secondary.material.value );
				const bool secondary_suppress_fill = secondary_is_outline && !cfg.secondary.filled.value;

				const bool suppress_fill = overlay_suppress_fill || primary_suppress_fill;

				if ( cfg.secondary.enabled.value )
				{
					if ( secondary_is_outline )
					{
						this->apply_overlay( primitive_buffer, original_fn, a1, target_scene_obj, scene_view, fade( cfg.secondary.color ), cfg.secondary.material, &cfg.secondary.glow );
					}
					else if ( !secondary_suppress_fill && !suppress_fill )
					{
						this->apply_layer( primitive_buffer, original_fn, a1, target_scene_obj, scene_view, fade( cfg.secondary.color ), cfg.secondary.material, &cfg.secondary.glow );
					}
				}

				const bool has_primary_fill = cfg.primary.enabled.value && !primary_is_outline && !suppress_fill;
				if ( has_primary_fill )
				{
					this->apply_layer( primitive_buffer, original_fn, a1, target_scene_obj, scene_view, fade( cfg.primary.color ), cfg.primary.material, &cfg.primary.glow );
				}

				const bool has_secondary_fill = cfg.secondary.enabled.value && !secondary_is_outline && !suppress_fill;
				const bool needs_original_base = !has_primary_fill && !has_secondary_fill &&
					( cfg.overlay.enabled.value || ( primary_is_outline && !primary_suppress_fill ) || force_original );

				if ( needs_original_base && !suppress_fill )
				{
					original_fn( a1, target_scene_obj, scene_view, primitive_buffer );
				}

				if ( primary_is_outline )
				{
					this->apply_overlay( primitive_buffer, original_fn, a1, target_scene_obj, scene_view, fade( cfg.primary.color ), cfg.primary.material, &cfg.primary.glow );
				}

				if ( cfg.overlay.enabled.value )
				{
					this->apply_overlay( primitive_buffer, original_fn, a1, target_scene_obj, scene_view, fade( cfg.overlay.color ), cfg.overlay.material, &cfg.overlay.glow );
				}
			};


		if ( !is_player && !is_ragdoll && !is_arms && !is_weapon )
		{
			if ( ( !settings::g_misc.m_camera.thirdperson.value && !features::misc::g_camera.is_freecam_active( ) ) || !is_local_attachment( systems::g_local.get( ).view_pawn( ) ) )
			{
				return false;
			}
			const auto& cfg = settings::g_esp.m_viewmodel.for_weapon( detail::weapon_definition( owner_entity ) );
			if ( !cfg.enabled.value || ( !cfg.primary.enabled.value && !cfg.secondary.enabled.value && !cfg.overlay.enabled.value ) ) return false;
			apply_config( cfg, scene_object );
			return true;
		}

		if ( is_arms || is_weapon )
		{
			const auto& cfg = is_arms ? settings::g_esp.m_viewmodel.arms :
				settings::g_esp.m_viewmodel.for_weapon( detail::active_weapon_definition( ) );
			if ( !cfg.enabled.value )
			{
				return false;
			}

			if ( !cfg.primary.enabled.value && !cfg.secondary.enabled.value && !cfg.overlay.enabled.value )
			{
				return false;
			}

			apply_config( cfg, scene_object );
			return true;
		}

		const auto local = systems::g_local.get( );
		const auto& chams_cfg = settings::g_esp.m_player.m_chams;

		const auto team = memory::read<int>( owner_entity + SCHEMA( "C_BaseEntity", "m_iTeamNum"_hash ) );
		const auto health = memory::read<int>( owner_entity + SCHEMA( "C_BaseEntity", "m_iHealth"_hash ) );

		const auto is_other_team = local.is_this_other_team( team );
		const auto is_local = owner_entity == local.view_pawn( );
		const auto is_dead = ( health <= 0 ) || is_ragdoll;

		if ( is_player && is_other_team && !is_dead && chams_cfg.backtrack.enabled.value )
		{
			if ( this->m_backtrack.has_active( owner_entity ) )
			{
				const auto bt_scene_object = this->m_backtrack.get_scene_object( owner_entity );
				if ( bt_scene_object )
				{
					const auto before = detail::read_primitive_buffer( primitive_buffer );
					const auto prev_count = before ? before->count() : -1;

					apply_config( chams_cfg.backtrack, bt_scene_object, true );

					const auto after = detail::read_primitive_buffer( primitive_buffer );
					const auto new_count = after ? after->count() : -1;
					if ( after && prev_count >= 0 && new_count > prev_count )
					{
						mark_range_last_fast( &*after, prev_count, new_count );
					}
				}
			}
		}

		if ( ( is_player || is_ragdoll ) && is_other_team && chams_cfg.onshot.enabled.value )
		{
			std::uintptr_t ragdoll_pawn = 0;
			if ( is_ragdoll )
			{
				const auto ragdoll_pawn_handle = memory::safe_read<std::uint32_t>( owner_entity + SCHEMA( "C_CSRagdoll", "m_hPlayerPawn"_hash ) ).value_or( 0 );
				if ( ragdoll_pawn_handle )
					ragdoll_pawn = systems::g_entities.lookup( ragdoll_pawn_handle );
			}

			const auto game_scene_node = memory::safe_read<std::uintptr_t>( owner_entity + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) ).value_or( 0 );
			const auto owner_model = game_scene_node ? memory::safe_read<std::uintptr_t>( game_scene_node + SCHEMA( "CSkeletonInstance", "m_modelState"_hash ) + SCHEMA( "CModelState", "m_hModel"_hash ) ).value_or( 0 ) : 0;

			this->m_onshot.render_for_entity( owner_entity, ragdoll_pawn, owner_model, primitive_buffer, original_fn, a1, scene_view, apply_config );
		}


		const settings::esp::chams_config* target{ nullptr };

		if ( is_dead )
		{
			if ( is_local )
			{
				target = &chams_cfg.local_ragdoll;
			}
			else if ( is_other_team )
			{
				target = &chams_cfg.enemy_ragdoll;
			}
			else
			{
				target = &chams_cfg.team_ragdoll;
			}
		}
		else
		{
			if ( is_local )
			{
				target = &chams_cfg.local;
			}
			else if ( is_other_team )
			{
				target = &chams_cfg.enemy;
			}
			else
			{
				target = &chams_cfg.team;
			}
		}

		if ( !target || !target->enabled.value )
		{
			return false;
		}

		if ( !target->primary.enabled.value && !target->secondary.enabled.value && !target->overlay.enabled.value )
		{
			return false;
		}

		{
			const auto flags = memory::safe_read<std::uint8_t>( scene_object + 0x78 );
			if ( flags ) {
				(void) memory::safe_write<std::uint8_t>(
					scene_object + 0x78,
					static_cast<std::uint8_t>( *flags & ~( 1u << 3 ) ) );
			}
		}

		if ( is_local )
		{
			if ( misc::g_other.is_alpha_changed( ) )
			{
				this->apply_clone( primitive_buffer, original_fn, a1, scene_object, scene_view, systems::materials::clone_type::translucent );

				if ( target->overlay.enabled.value )
				{
					this->apply_overlay( primitive_buffer, original_fn, a1, scene_object, scene_view, target->overlay.color, target->overlay.material, &target->overlay.glow );
				}
			}
			else
			{
				apply_config( *target, scene_object );
			}

			return true;
		}

		apply_config( *target, scene_object );
		return true;
	}



	void chams::on_sort_primitives( std::uintptr_t entries, std::uint32_t count )
	{
		// scenesystem.dll already sorts primitives by draw_order (0x58) and primitive_draw_last (0x62) natively.
		// Manual post-sort partition over thousands of elements in every pass causes massive CPU churn and FPS drops.
	}

	void chams::backtrack::update( )
	{
		const auto& cfg = settings::g_esp.m_player.m_chams;
		const auto local = systems::g_local.get( );

		if ( !local.is_alive || !cfg.backtrack.enabled.value )
		{
			for ( auto it = this->m_objects.begin( ); it != this->m_objects.end( ); )
			{
				it->second.destroy( );
				it = this->m_objects.erase( it );
			}

			return;
		}

		const auto players = systems::g_entities.get_by_type( systems::entities::type::player );

		std::unordered_set<std::uintptr_t> valid_pawns;

		for ( const auto& p : players )
		{
			if ( !p.ptr || p.ptr == local.controller )
			{
				continue;
			}

			const auto pawn_handle = memory::read<std::uint32_t>( p.ptr + SCHEMA( "CBasePlayerController", "m_hPawn"_hash ) );
			const auto pawn = systems::g_entities.lookup( pawn_handle );

			if ( pawn && pawn != local.pawn )
			{
				valid_pawns.insert( pawn );
			}
		}

		for ( auto it = this->m_objects.begin( ); it != this->m_objects.end( ); )
		{
			if ( !valid_pawns.contains( it->first ) )
			{
				it->second.destroy( );
				it = this->m_objects.erase( it );
			}
			else
			{
				++it;
			}
		}

		std::unordered_set<std::uintptr_t> active;

		for ( const auto& p : players )
		{
			if ( !p.ptr || p.ptr == local.controller )
			{
				continue;
			}

			if ( !memory::read<bool>( p.ptr + SCHEMA( "CCSPlayerController", "m_bPawnIsAlive"_hash ) ) )
			{
				auto it = this->m_objects.find( p.ptr );
				if ( it != this->m_objects.end( ) )
				{
					it->second.destroy( );
					this->m_objects.erase( it );
				}

				continue;
			}

			const auto pawn_handle = memory::read<std::uint32_t>( p.ptr + SCHEMA( "CBasePlayerController", "m_hPawn"_hash ) );
			const auto pawn = systems::g_entities.lookup( pawn_handle );

			if ( !pawn || pawn == local.pawn )
			{
				continue;
			}

			const auto team = memory::read<int>( pawn + SCHEMA( "C_BaseEntity", "m_iTeamNum"_hash ) );
			if ( !local.is_this_other_team( team ) )
			{
				continue;
			}

			const auto health = memory::read<int>( pawn + SCHEMA( "C_BaseEntity", "m_iHealth"_hash ) );
			if ( health <= 0 )
			{
				auto it = this->m_objects.find( pawn );
				if ( it != this->m_objects.end( ) )
				{
					it->second.destroy( );
					this->m_objects.erase( it );
				}

				continue;
			}

			const auto oldest = combat::g_shared.lc( ).get_oldest_was_valid( pawn );
			if ( !oldest )
			{
				auto it = this->m_objects.find( pawn );
				if ( it != this->m_objects.end( ) )
				{
					it->second.destroy( );
					this->m_objects.erase( it );
				}

				continue;
			}

			const auto game_scene_node = memory::read<std::uintptr_t>( pawn + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) );
			if ( game_scene_node )
			{
				if ( oldest->origin.distance( memory::read<math::vector3>( game_scene_node + SCHEMA( "CGameSceneNode", "m_vecAbsOrigin"_hash ) ) ) < 0.25f )
				{
					auto it = this->m_objects.find( pawn );
					if ( it != this->m_objects.end( ) )
					{
						it->second.destroy( );
						this->m_objects.erase( it );
					}

					continue;
				}
			}

			auto& obj = this->m_objects[ pawn ];
			if ( !obj.scene_object )
			{
				obj.create( pawn );
			}

			if ( !obj.scene_object )
			{
				continue;
			}

			active.insert( pawn );
			obj.active = true;
			obj.setup_bones( oldest->bones, oldest->bone_count );
		}

		for ( auto it = this->m_objects.begin( ); it != this->m_objects.end( ); )
		{
			if ( !active.contains( it->first ) )
			{
				it->second.destroy( );
				it = this->m_objects.erase( it );
			}
			else
			{
				++it;
			}
		}
	}

	void chams::backtrack::shutdown( bool destroy_objects )
	{
		if ( destroy_objects )
		{
			for ( auto& [pawn, obj] : this->m_objects )
			{
				obj.destroy( );
			}
		}

		this->m_objects.clear( );
	}

	bool chams::backtrack::is_active( std::uintptr_t scene_object ) const
	{
		for ( const auto& [pawn, obj] : this->m_objects )
		{
			if ( obj.scene_object == scene_object && obj.active )
			{
				return true;
			}
		}

		return false;
	}

	bool chams::backtrack::has_active( std::uintptr_t pawn ) const
	{
		auto it = this->m_objects.find( pawn );
		return it != this->m_objects.end( ) && it->second.scene_object && it->second.active;
	}

	std::uintptr_t chams::backtrack::get_scene_object( std::uintptr_t pawn ) const
	{
		auto it = this->m_objects.find( pawn );
		if ( it != this->m_objects.end( ) )
		{
			return it->second.scene_object;
		}

		return 0;
	}

	void chams::backtrack::object::create( std::uintptr_t target_pawn )
	{
		this->pawn = target_pawn;
		this->scene_object = 0;

		const auto game_scene_node = memory::read<std::uintptr_t>( target_pawn + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) );
		if ( !game_scene_node )
		{
			return;
		}

		auto temp{ 0 };

		const auto world_group_id = memory::call<int*>(PATTERN (patterns::get_world_group_id), game_scene_node, &temp );
		if ( !world_group_id )
		{
			return;
		}

		const auto render_game_system = memory::read<std::uintptr_t>( addresses::globals::render_game_system_storage );
		if ( !render_game_system )
		{
			return;
		}

		const auto world_group_handle = memory::call<std::uintptr_t>(PATTERN (patterns::get_world_group_handle), render_game_system, *world_group_id );
		if ( !world_group_handle )
		{
			return;
		}

		const auto flags = ( *world_group_id != 0 ) ? 0x2000000000ll : 0x2000000008ll;
		const auto model_handle = memory::read<std::uintptr_t>( game_scene_node + SCHEMA( "CSkeletonInstance", "m_modelState"_hash ) + SCHEMA( "CModelState", "m_hModel"_hash ) );
		const auto node_to_world = game_scene_node + SCHEMA( "CGameSceneNode", "m_nodeToWorld"_hash );

		__m128 copy[ 2 ]{};
		copy[ 0 ] = *reinterpret_cast< __m128* >( node_to_world );
		copy[ 1 ] = *reinterpret_cast< __m128* >( node_to_world + 16 );

		this->scene_object = memory::call_vfunc<std::uintptr_t>( addresses::globals::mesh_system, 20, model_handle, &copy, "AnimatableSceneObjectDesc", flags, 0x4100000001ll, world_group_handle );
		if ( !this->scene_object )
		{
			return;
		}

		memory::write<std::uintptr_t>( this->scene_object + 0x110, game_scene_node );
		memory::write<int>( this->scene_object + 0xc0, -1 );

		const auto model_data = memory::read<std::uintptr_t>( model_handle );
		if ( model_data )
		{
			const auto has_force_lod = ( memory::read<std::uint32_t>( model_data + 16 ) & 0x400 ) != 0 || ( memory::read<std::uint32_t>( model_data + 20 ) & 0x400 ) != 0;
			auto lod = memory::read<std::uint8_t>( this->scene_object + 0x9a );
			lod = has_force_lod ? ( lod | 0x10 ) : ( lod & 0xef );
			memory::write( this->scene_object + 0x9a, lod );
		}
	}

	void chams::backtrack::object::destroy( )
	{
		if ( !this->scene_object )
		{
			return;
		}

		const auto flags = memory::safe_read<std::uint64_t>( this->scene_object + 128 );
		if ( !flags || ( *flags & 0x4000000000000000ull ) )
		{
			this->scene_object = 0;
			return;
		}

		if ( addresses::globals::scene_system )
		{
			memory::call_vfunc<void>( addresses::globals::scene_system, 16, this->scene_object );
		}
		this->scene_object = 0;
	}

	void chams::backtrack::object::setup_bones( systems::bones::data* bones, int count ) const
	{
		if ( !this->scene_object )
		{
			return;
		}

		const auto obj_bone_count = memory::read<int>( this->scene_object + 0xd0 );
		const auto render_bones = memory::read<std::uintptr_t>( this->scene_object + 0xd8 );

		if ( !render_bones || obj_bone_count <= 0 )
		{
			return;
		}

		const auto write_count = std::min( count, obj_bone_count );

		for ( auto i = 0; i < write_count; i++ )
		{
			const auto& b = bones[ i ];
			const auto dst = render_bones + ( static_cast< std::size_t >( i ) * 48 );

			const auto bxx = b.rotation.x * b.rotation.x;
			const auto byy = b.rotation.y * b.rotation.y;
			const auto bzz = b.rotation.z * b.rotation.z;
			const auto bxy = b.rotation.x * b.rotation.y;
			const auto bxz = b.rotation.x * b.rotation.z;
			const auto byz = b.rotation.y * b.rotation.z;
			const auto bwx = b.rotation.w * b.rotation.x;
			const auto bwy = b.rotation.w * b.rotation.y;
			const auto bwz = b.rotation.w * b.rotation.z;

			memory::write<float>( dst + 0, 1.0f - 2.0f * ( byy + bzz ) );
			memory::write<float>( dst + 4, 2.0f * ( bxy - bwz ) );
			memory::write<float>( dst + 8, 2.0f * ( bxz + bwy ) );
			memory::write<float>( dst + 12, b.position.x );
			memory::write<float>( dst + 16, 2.0f * ( bxy + bwz ) );
			memory::write<float>( dst + 20, 1.0f - 2.0f * ( bxx + bzz ) );
			memory::write<float>( dst + 24, 2.0f * ( byz - bwx ) );
			memory::write<float>( dst + 28, b.position.y );
			memory::write<float>( dst + 32, 2.0f * ( bxz - bwy ) );
			memory::write<float>( dst + 36, 2.0f * ( byz + bwx ) );
			memory::write<float>( dst + 40, 1.0f - 2.0f * ( bxx + byy ) );
			memory::write<float>( dst + 44, b.position.z );
		}
	}

	void chams::onshot::push (std::uintptr_t pawn, const systems::bones::data* bones, int bone_count) {
		const auto& cfg = settings::g_esp.m_player.m_chams;
		if (!cfg.onshot.enabled.value || !pawn)
			return;

		const auto game_scene_node = memory::safe_read<std::uintptr_t>( pawn + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) ).value_or( 0 );
		if ( !game_scene_node )
			return;

		auto temp{ 0 };
		const auto world_group_id = memory::call<int*>( PATTERN (patterns::get_world_group_id), game_scene_node, &temp );
		if ( !world_group_id )
			return;

		const auto render_game_system = memory::read<std::uintptr_t>( addresses::globals::render_game_system_storage );
		if ( !render_game_system )
			return;

		const auto world_group_handle = memory::call<std::uintptr_t>( PATTERN (patterns::get_world_group_handle), render_game_system, *world_group_id );
		if ( !world_group_handle )
			return;

		const auto model_handle = memory::safe_read<std::uintptr_t>( game_scene_node + SCHEMA( "CSkeletonInstance", "m_modelState"_hash ) + SCHEMA( "CModelState", "m_hModel"_hash ) ).value_or( 0 );
		if ( !model_handle )
			return;

		const auto flags = ( *world_group_id != 0 ) ? 0x2000000000ll : 0x2000000008ll;
		const auto node_to_world = game_scene_node + SCHEMA( "CGameSceneNode", "m_nodeToWorld"_hash );

		pending_entry entry{};
		entry.pawn = pawn;
		entry.model_handle = model_handle;
		entry.world_group_handle = world_group_handle;
		entry.flags = flags;
		entry.node_to_world[ 0 ] = *reinterpret_cast< const __m128* >( node_to_world );
		entry.node_to_world[ 1 ] = *reinterpret_cast< const __m128* >( node_to_world + 16 );

		const auto bone_cache = memory::safe_read<std::uintptr_t>( game_scene_node + SCHEMA( "CSkeletonInstance", "m_modelState"_hash ) + 0x80 ).value_or( 0 );
		const auto live_count = memory::safe_read<int>( game_scene_node + SCHEMA( "CSkeletonInstance", "m_modelState"_hash ) + 0x8c ).value_or( 0 );
		if ( bone_cache && live_count > 0 )
		{
			const auto count = std::clamp( live_count, 0, 128 );
			entry.bone_count = count;
			for ( int i = 0; i < count; ++i )
			{
				const auto b = memory::safe_read<systems::bones::data>( bone_cache + i * sizeof( systems::bones::data ) );
				if ( b )
					entry.bones[ i ] = *b;
			}
		}

		if ( entry.bone_count <= 0 )
		{
			const auto records = combat::g_shared.lc( ).get_valid_records( pawn );
			if ( !records.empty( ) && records.front( )->bone_count > 0 )
			{
				auto* record = records.front( );
				const auto count = std::clamp( record->bone_count, 0, 128 );
				entry.bone_count = count;
				std::copy_n( record->bones, count, entry.bones.begin( ) );
			}
			else
			{
				const auto oldest = combat::g_shared.lc( ).get_oldest_was_valid( pawn );
				if ( oldest && oldest->bone_count > 0 )
				{
					const auto count = std::clamp( oldest->bone_count, 0, 128 );
					entry.bone_count = count;
					std::copy_n( oldest->bones, count, entry.bones.begin( ) );
				}
			}
		}

		if ( entry.bone_count <= 0 && bones && bone_count > 0 )
		{
			const auto count = std::clamp( bone_count, 0, 128 );
			entry.bone_count = count;
			std::copy_n( bones, count, entry.bones.begin( ) );
		}

		if ( entry.bone_count <= 0 )
			return;

		std::lock_guard lock( this->m_mtx );
		this->m_pending.push_back( std::move( entry ) );
	}

	void chams::onshot::update () {
		const auto& cfg = settings::g_esp.m_player.m_chams;
		std::lock_guard lock( this->m_mtx );

		if (!cfg.onshot.enabled.value) {
			if ( addresses::globals::scene_system ) {
				for (auto& e : this->m_entries) {
					if (e.scene_object) {
						const auto s_flags = memory::safe_read<std::uint64_t>( e.scene_object + 128 );
						if ( s_flags && !( *s_flags & 0x4000000000000000ull ) ) {
							memory::call_vfunc<void>( addresses::globals::scene_system, 16, e.scene_object );
						}
					}
				}
			}
			this->m_entries.clear ();
			this->m_pending.clear ();
			return;
		}

		const auto global_vars = memory::read<std::uintptr_t> (addresses::globals::global_vars);
		const auto current_time = global_vars ? memory::read<float> (global_vars + 0x30) : 0.0f;

		for (auto& pending : this->m_pending) {
			if ( !addresses::globals::mesh_system || !pending.model_handle || !pending.world_group_handle )
				continue;

			__m128 copy[ 2 ]{};
			copy[ 0 ] = pending.node_to_world[ 0 ];
			copy[ 1 ] = pending.node_to_world[ 1 ];

			const auto scene_obj = memory::call_vfunc<std::uintptr_t>(
				addresses::globals::mesh_system,
				20,
				pending.model_handle,
				&copy,
				"AnimatableSceneObjectDesc",
				pending.flags,
				0x4100000001ll,
				pending.world_group_handle
			);

			if ( !scene_obj )
				continue;

			memory::write<std::uintptr_t>( scene_obj + 0x110, 0 );
			memory::write<int>( scene_obj + 0xc0, -1 );

			const auto model_data = memory::read<std::uintptr_t>( pending.model_handle );
			if ( model_data ) {
				const auto has_force_lod = ( memory::read<std::uint32_t>( model_data + 16 ) & 0x400 ) != 0 || ( memory::read<std::uint32_t>( model_data + 20 ) & 0x400 ) != 0;
				auto lod = memory::read<std::uint8_t>( scene_obj + 0x9a );
				lod = has_force_lod ? ( lod | 0x10 ) : ( lod & 0xef );
				memory::write( scene_obj + 0x9a, lod );
			}

			const auto obj_bone_count = memory::read<int>( scene_obj + 0xd0 );
			const auto render_bones = memory::read<std::uintptr_t>( scene_obj + 0xd8 );
			if ( render_bones && obj_bone_count > 0 ) {
				const auto write_count = std::min( pending.bone_count, obj_bone_count );
				for ( int i = 0; i < write_count; ++i ) {
					const auto& b = pending.bones[ i ];
					const auto dst = render_bones + ( static_cast< std::size_t >( i ) * 48 );

					const auto bxx = b.rotation.x * b.rotation.x;
					const auto byy = b.rotation.y * b.rotation.y;
					const auto bzz = b.rotation.z * b.rotation.z;
					const auto bxy = b.rotation.x * b.rotation.y;
					const auto bxz = b.rotation.x * b.rotation.z;
					const auto byz = b.rotation.y * b.rotation.z;
					const auto bwx = b.rotation.w * b.rotation.x;
					const auto bwy = b.rotation.w * b.rotation.y;
					const auto bwz = b.rotation.w * b.rotation.z;

					memory::write<float>( dst + 0, 1.0f - 2.0f * ( byy + bzz ) );
					memory::write<float>( dst + 4, 2.0f * ( bxy - bwz ) );
					memory::write<float>( dst + 8, 2.0f * ( bxz + bwy ) );
					memory::write<float>( dst + 12, b.position.x );
					memory::write<float>( dst + 16, 2.0f * ( bxy + bwz ) );
					memory::write<float>( dst + 20, 1.0f - 2.0f * ( bxx + bzz ) );
					memory::write<float>( dst + 24, 2.0f * ( byz - bwx ) );
					memory::write<float>( dst + 28, b.position.y );
					memory::write<float>( dst + 32, 2.0f * ( bxz - bwy ) );
					memory::write<float>( dst + 36, 2.0f * ( byz + bwx ) );
					memory::write<float>( dst + 40, 1.0f - 2.0f * ( bxx + byy ) );
					memory::write<float>( dst + 44, b.position.z );
				}
			}

			this->m_entries.push_back( { scene_obj, pending.pawn, pending.model_handle, current_time } );
		}
		this->m_pending.clear ();

		const auto fade_time = cfg.onshot_fade_time.value;

		for (auto it = this->m_entries.begin (); it != this->m_entries.end (); ) {
			if (current_time - it->spawn_time >= fade_time) {
				if ( it->scene_object && addresses::globals::scene_system ) {
					const auto s_flags = memory::safe_read<std::uint64_t>( it->scene_object + 128 );
					if ( s_flags && !( *s_flags & 0x4000000000000000ull ) ) {
						memory::call_vfunc<void>( addresses::globals::scene_system, 16, it->scene_object );
					}
				}
				it = this->m_entries.erase (it);
			} else {
				++it;
			}
		}
	}

	void chams::onshot::shutdown (bool destroy_objects) {
		std::lock_guard lock( this->m_mtx );
		if (destroy_objects && addresses::globals::scene_system) {
			for (auto& e : this->m_entries) {
				if ( e.scene_object ) {
					const auto s_flags = memory::safe_read<std::uint64_t>( e.scene_object + 128 );
					if ( s_flags && !( *s_flags & 0x4000000000000000ull ) ) {
						memory::call_vfunc<void>( addresses::globals::scene_system, 16, e.scene_object );
					}
				}
			}
		}
		this->m_entries.clear ();
		this->m_pending.clear ();
	}

	bool chams::onshot::has_active (std::uintptr_t pawn) const {
		std::lock_guard lock( this->m_mtx );
		if ( pawn ) {
			for ( const auto& e : this->m_entries ) {
				if ( e.pawn == pawn && e.scene_object )
					return true;
			}
			return false;
		}
		return !this->m_entries.empty ();
	}

	bool chams::onshot::is_active (std::uintptr_t scene_object) const {
		if ( !scene_object )
			return false;
		std::lock_guard lock( this->m_mtx );
		for (const auto& e : this->m_entries)
			if (e.scene_object == scene_object)
				return true;
		return false;
	}

	std::uintptr_t chams::onshot::get_scene_object (std::uintptr_t pawn) const {
		std::lock_guard lock( this->m_mtx );
		if ( pawn ) {
			for ( const auto& e : this->m_entries ) {
				if ( e.pawn == pawn )
					return e.scene_object;
			}
		}
		if (!this->m_entries.empty ())
			return this->m_entries.back ().scene_object;
		return 0;
	}

	float chams::onshot::get_alpha (std::uintptr_t scene_object) const {
		std::lock_guard lock( this->m_mtx );
		for (const auto& e : this->m_entries) {
			if (e.scene_object == scene_object) {
				const auto global_vars = memory::read<std::uintptr_t> (addresses::globals::global_vars);
				const auto current_time = global_vars ? memory::read<float> (global_vars + 0x30) : 0.0f;
				const auto fade_time = settings::g_esp.m_player.m_chams.onshot_fade_time.value;

				if (fade_time <= 0.0f)
					return 0.0f;

				const auto elapsed = current_time - e.spawn_time;
				return std::clamp (1.0f - (elapsed / fade_time), 0.0f, 1.0f);
			}
		}
		return 0.0f;
	}

	void chams::apply_layer( std::uintptr_t primitive_buffer, void( __fastcall* original_fn )( std::uintptr_t, std::uintptr_t, std::uintptr_t, std::uintptr_t ), std::uintptr_t a1, std::uintptr_t scene_object, std::uintptr_t scene_view, const xdraw::color& color, settings::esp::cham_ids material_id, const settings::esp::outline_glow_config* glow_cfg )
	{
		const auto before = detail::read_primitive_buffer( primitive_buffer );
		const auto prev_count = before ? before->count() : -1;

		original_fn( a1, scene_object, scene_view, primitive_buffer );

		const auto after = detail::read_primitive_buffer( primitive_buffer );
		const auto new_count = after ? after->count() : -1;
		if ( !after || prev_count < 0 || prev_count >= new_count )
		{
			return;
		}

		std::uintptr_t material = 0;
		if ( glow_cfg && ( material_id == settings::esp::cham_ids::outline_glow || material_id == settings::esp::cham_ids::outline_glow_ignorez ) )
		{
			const bool is_iz = ( material_id == settings::esp::cham_ids::outline_glow_ignorez );
			material = systems::materials::get_outline_glow( *glow_cfg, is_iz );
		}
		if ( !material )
		{
			material = systems::materials::find( material_id );
		}
		if ( !material )
		{
			return;
		}

		auto draw_color = color;
		const auto pulse = glow_cfg ? glow_cfg->pulse_speed.value : settings::g_esp.m_outline_glow.pulse_speed.value;
		if ( ( material_id == settings::esp::cham_ids::outline_glow || material_id == settings::esp::cham_ids::outline_glow_ignorez ) &&
		     pulse > 0.01f )
		{
			const auto now = std::chrono::steady_clock::now( );
			const auto sec = std::chrono::duration<float>( now.time_since_epoch( ) ).count( );
			const auto wave = 0.5f + 0.5f * std::sin( sec * pulse * 4.0f );
			draw_color.r = static_cast<std::uint8_t>( draw_color.r * ( 0.3f + 0.7f * wave ) );
			draw_color.g = static_cast<std::uint8_t>( draw_color.g * ( 0.3f + 0.7f * wave ) );
			draw_color.b = static_cast<std::uint8_t>( draw_color.b * ( 0.3f + 0.7f * wave ) );
			draw_color.a = static_cast<std::uint8_t>( draw_color.a * ( 0.3f + 0.7f * wave ) );
		}

		const auto color_val = static_cast<std::uint32_t>( draw_color );
		__try
		{
			for ( auto i = prev_count; i < new_count; ++i )
			{
				const auto primitive = after->at_fast( i );
				if ( primitive )
				{
					detail::replace_primitive_fast( primitive, material, color_val );
				}
			}
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
		}
	}

	void chams::apply_overlay( std::uintptr_t primitive_buffer, void( __fastcall* original_fn )( std::uintptr_t, std::uintptr_t, std::uintptr_t, std::uintptr_t ), std::uintptr_t a1, std::uintptr_t scene_object, std::uintptr_t scene_view, const xdraw::color& color, settings::esp::cham_ids material_id, const settings::esp::outline_glow_config* glow_cfg )
	{
		std::uintptr_t material = 0;
		if ( glow_cfg && ( material_id == settings::esp::cham_ids::outline_glow || material_id == settings::esp::cham_ids::outline_glow_ignorez ) )
		{
			const bool is_iz = ( material_id == settings::esp::cham_ids::outline_glow_ignorez );
			material = systems::materials::get_outline_glow( *glow_cfg, is_iz );
		}
		if ( !material )
		{
			material = systems::materials::find( material_id );
		}
		if ( !material )
		{
			return;
		}

		const auto before = detail::read_primitive_buffer( primitive_buffer );
		const auto prev_count = before ? before->count() : -1;

		original_fn( a1, scene_object, scene_view, primitive_buffer );

		const auto after = detail::read_primitive_buffer( primitive_buffer );
		const auto new_count = after ? after->count() : -1;
		if ( !after || prev_count < 0 || prev_count >= new_count )
		{
			return;
		}

		auto draw_color = color;
		const auto pulse = glow_cfg ? glow_cfg->pulse_speed.value : settings::g_esp.m_outline_glow.pulse_speed.value;
		if ( ( material_id == settings::esp::cham_ids::outline_glow || material_id == settings::esp::cham_ids::outline_glow_ignorez ) &&
		     pulse > 0.01f )
		{
			const auto now = std::chrono::steady_clock::now( );
			const auto sec = std::chrono::duration<float>( now.time_since_epoch( ) ).count( );
			const auto wave = 0.5f + 0.5f * std::sin( sec * pulse * 4.0f );
			draw_color.r = static_cast<std::uint8_t>( draw_color.r * ( 0.3f + 0.7f * wave ) );
			draw_color.g = static_cast<std::uint8_t>( draw_color.g * ( 0.3f + 0.7f * wave ) );
			draw_color.b = static_cast<std::uint8_t>( draw_color.b * ( 0.3f + 0.7f * wave ) );
			draw_color.a = static_cast<std::uint8_t>( draw_color.a * ( 0.3f + 0.7f * wave ) );
		}

		const auto color_val = static_cast<std::uint32_t>( draw_color );
		__try
		{
			for ( auto i = prev_count; i < new_count; ++i )
			{
				const auto primitive = after->at_fast( i );
				if ( primitive )
				{
					detail::replace_primitive_fast( primitive, material, color_val );
					detail::mark_primitive_last_fast( primitive );
				}
			}
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
		}

		this->add_overlay_material( material );
	}

	void chams::apply_clone( std::uintptr_t primitive_buffer, void( __fastcall* original_fn )( std::uintptr_t, std::uintptr_t, std::uintptr_t, std::uintptr_t ), std::uintptr_t a1, std::uintptr_t scene_object, std::uintptr_t scene_view, systems::materials::clone_type type )
	{
		const auto before = detail::read_primitive_buffer( primitive_buffer );
		const auto prev_count = before ? before->count() : -1;

		original_fn( a1, scene_object, scene_view, primitive_buffer );

		const auto after = detail::read_primitive_buffer( primitive_buffer );
		const auto new_count = after ? after->count() : -1;
		if ( !after || prev_count < 0 || prev_count >= new_count )
		{
			return;
		}

		__try
		{
			for ( auto i = prev_count; i < new_count; ++i )
			{
				const auto primitive = after->at_fast( i );
				if ( !primitive )
				{
					continue;
				}

				const auto orig_mat = *reinterpret_cast<const std::uintptr_t*>(
					primitive + detail::primitive_material_offset );

				if ( !orig_mat )
				{
					continue;
				}

				const auto clone = systems::materials::get_or_create_clone( orig_mat, type );
				if ( !clone )
				{
					continue;
				}

				*reinterpret_cast<std::uintptr_t*>(
					primitive + detail::primitive_material_offset ) = clone;
			}
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
		}
	}

	bool chams::is_overlay_material( std::uintptr_t mat ) const
	{
		if ( !mat )
		{
			return false;
		}

		if ( mat == systems::materials::find( settings::esp::cham_ids::outline_glow ) ||
		     mat == systems::materials::find( settings::esp::cham_ids::outline_glow_ignorez ) ||
		     mat == systems::materials::find( settings::esp::cham_ids::outlines ) ||
		     mat == systems::materials::find( settings::esp::cham_ids::outlines_ignorez ) ||
		     mat == systems::materials::find( settings::esp::cham_ids::glow ) ||
		     mat == systems::materials::find( settings::esp::cham_ids::glow_ignorez ) )
		{
			return true;
		}

		const auto count = this->m_overlay_material_count.load( std::memory_order_acquire );

		for ( auto i = 0; i < count; ++i )
		{
			if ( this->m_overlay_materials[ i ].load( std::memory_order_relaxed ) == mat )
			{
				return true;
			}
		}

		return false;
	}

	void chams::add_overlay_material( std::uintptr_t mat )
	{
		const auto count = this->m_overlay_material_count.load( std::memory_order_acquire );

		for ( auto i = 0; i < count; ++i )
		{
			if ( this->m_overlay_materials[ i ].load( std::memory_order_relaxed ) == mat )
			{
				return;
			}
		}

		const auto idx = this->m_overlay_material_count.fetch_add( 1, std::memory_order_acq_rel );

		if ( idx < k_max_overlay_materials )
		{
			this->m_overlay_materials[ idx ].store( mat, std::memory_order_release );
		}
		else
		{
			this->m_overlay_material_count.fetch_sub( 1, std::memory_order_release );
		}
	}

} // namespace features::esp::player
