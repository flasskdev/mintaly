#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <utilities/diag.hpp>
#include <utilities/logging/logging.hpp>
#include <protection/game_addresses.hpp>
#include "../systems.hpp"

namespace {

	constexpr int k_max_subtick_moves{ 32 }; // the subtick strafer writes up to 32 steps per tick

	[[nodiscard]] bool valid_runtime_pointer( std::uintptr_t address ) noexcept
	{
		return address >= 0x10000ull &&
			address != ( std::numeric_limits<std::uintptr_t>::max )( ) &&
			address <= 0x00007FFFFFFFFFFFull;
	}

	[[nodiscard]] bool read_subtick_rep(
		proto::repeated_ptr_field<proto::subtick_move_step>* moves,
		proto::repeated_ptr_field<proto::subtick_move_step>::rep_t*& rep,
		int& current_size,
		int& allocated_size ) noexcept
	{
		rep = nullptr;
		current_size = 0;
		allocated_size = 0;
		if ( !moves )
			return false;

		const auto raw = reinterpret_cast<std::uintptr_t>( moves );
		const auto current = memory::safe_read<int>( raw + offsetof( proto::repeated_ptr_field<proto::subtick_move_step>, m_current_size ) );
		const auto rep_value = memory::safe_read<proto::repeated_ptr_field<proto::subtick_move_step>::rep_t*>(
			raw + offsetof( proto::repeated_ptr_field<proto::subtick_move_step>, m_rep ) );
		if ( !current || !rep_value || *current < 0 || *current > k_max_subtick_moves )
			return false;

		current_size = *current;
		rep = *rep_value;
		if ( !rep )
			return current_size == 0;
		if ( !valid_runtime_pointer( reinterpret_cast<std::uintptr_t>( rep ) ) )
			return false;

		const auto allocated = memory::safe_read<int>( reinterpret_cast<std::uintptr_t>( rep ) +
			offsetof( proto::repeated_ptr_field<proto::subtick_move_step>::rep_t, allocated_size ) );
		if ( !allocated || *allocated < current_size || *allocated <= 0 || *allocated > 1024 )
			return false;

		allocated_size = *allocated;
		return true;
	}

	[[nodiscard]] void* read_subtick_element(
		proto::repeated_ptr_field<proto::subtick_move_step>::rep_t* rep,
		int index ) noexcept
	{
		if ( !rep || index < 0 || index >= k_max_subtick_moves )
			return nullptr;

		const auto address = reinterpret_cast<std::uintptr_t>( rep ) +
			offsetof( proto::repeated_ptr_field<proto::subtick_move_step>::rep_t, elements ) +
			static_cast<std::size_t>( index ) * sizeof( void* );
		return memory::safe_read<void*>( address ).value_or( nullptr );
	}

	[[nodiscard]] bool reset_subtick_step( void* element, proto::subtick_move_step*& step ) noexcept
	{
		step = nullptr;
		if ( !valid_runtime_pointer( reinterpret_cast<std::uintptr_t>( element ) ) )
			return false;

		const auto step_address = reinterpret_cast<std::uintptr_t>( element ) + proto::message_impl_offset;
		if ( !valid_runtime_pointer( step_address ) )
			return false;

		// Do NOT zero the message here - the game's allocator returns a properly
		// initialized protobuf message with vtable/arena pointer intact. Zeroing
		// the full struct (including protobuf header fields) corrupts the heap.
		// Fields we need are overwritten in emit_step/apply_landing_steps.

		step = proto::impl_ptr<proto::subtick_move_step>( element );
		if ( !step || !valid_runtime_pointer( reinterpret_cast<std::uintptr_t>( step ) ) )
			return false;

		return true;
	}

	[[nodiscard]] bool sort_subtick_moves_safely(
		proto::repeated_ptr_field<proto::subtick_move_step>* moves ) noexcept
	{
		proto::repeated_ptr_field<proto::subtick_move_step>::rep_t* rep{};
		int current_size{};
		int allocated_size{};
		if ( !read_subtick_rep( moves, rep, current_size, allocated_size ) ||
			!rep || current_size <= 1 || allocated_size < current_size )
			return false;

		std::array<void*, k_max_subtick_moves> elements{};
		for ( auto i = 0; i < current_size; ++i )
		{
			elements[ i ] = read_subtick_element( rep, i );
			if ( !valid_runtime_pointer( reinterpret_cast<std::uintptr_t>( elements[ i ] ) ) )
				return false;
		}

		std::stable_sort( elements.begin( ), elements.begin( ) + current_size,
			[]( void* lhs, void* rhs )
			{
				const auto left_when = memory::safe_read<float>(
					reinterpret_cast<std::uintptr_t>( lhs ) + proto::message_impl_offset +
					offsetof( proto::subtick_move_step, m_when ) ).value_or( 0.0f );
				const auto right_when = memory::safe_read<float>(
					reinterpret_cast<std::uintptr_t>( rhs ) + proto::message_impl_offset +
					offsetof( proto::subtick_move_step, m_when ) ).value_or( 0.0f );
				return left_when < right_when;
			} );

		for ( auto i = 0; i < current_size; ++i )
		{
			const auto address = reinterpret_cast<std::uintptr_t>( rep ) +
				offsetof( proto::repeated_ptr_field<proto::subtick_move_step>::rep_t, elements ) +
				static_cast<std::size_t>( i ) * sizeof( void* );
			if ( !memory::safe_write<void*>( address, elements[ i ] ) )
				return false;
		}
		return true;
	}

} // namespace

namespace systems {

	void input::update( )
	{
		const auto local_controller = memory::read<std::uintptr_t>( addresses::globals::local_player_controller );
		if ( !local_controller )
		{
			this->m_current_cmd = nullptr;
			return;
		}

		this->m_current_cmd = this->get_current_cmd( local_controller );
	}

	bool input::has_analog_subticks( proto::base_usercmd_pb* base ) const
	{
		if ( !base )
		{
			return false;
		}

		auto* const moves = base->mutable_subtick_moves( );
		proto::repeated_ptr_field<proto::subtick_move_step>::rep_t* rep{};
		int current_size{};
		int allocated_size{};
		if ( !read_subtick_rep( moves, rep, current_size, allocated_size ) || !rep )
			return false;

		for ( int i = 0; i < current_size; ++i )
		{
			const auto element = read_subtick_element( rep, i );
			if ( !valid_runtime_pointer( reinterpret_cast<std::uintptr_t>( element ) ) )
				continue;
			const auto step = proto::impl_ptr<proto::subtick_move_step>( element );
			if ( step && ( ( step->m_has_bits.test( 0x8 ) && step->analog_forward_delta( ) != 0.0f ) ||
				( step->m_has_bits.test( 0x10 ) && step->analog_left_delta( ) != 0.0f ) ) )
			{
				return true;
			}
		}
		return false;
	}

	float input::max_subtick_when( proto::base_usercmd_pb* base ) const
	{
		if ( !base )
			return 0.0f;

		proto::repeated_ptr_field<proto::subtick_move_step>::rep_t* rep{};
		int current_size{};
		int allocated_size{};
		if ( !read_subtick_rep( base->mutable_subtick_moves( ), rep, current_size, allocated_size ) || !rep )
			return 0.0f;

		float max_when = 0.0f;
		for ( int i = 0; i < current_size; ++i )
		{
			const auto element = read_subtick_element( rep, i );
			if ( !valid_runtime_pointer( reinterpret_cast<std::uintptr_t>( element ) ) )
				continue;

			const auto when = memory::safe_read<float>(
				reinterpret_cast<std::uintptr_t>( element ) + proto::message_impl_offset +
				offsetof( proto::subtick_move_step, m_when ) ).value_or( 0.0f );
			if ( std::isfinite( when ) )
				max_when = std::fmaxf( max_when, std::clamp( when, 0.0f, 1.0f ) );
		}

		return max_when;
	}

	void input::apply( )
	{
		const auto local = systems::g_local.get ();

		if ( !this->m_current_cmd ) {
			return;
		}

		const auto base = this->m_current_cmd->csgo_user_cmd.mutable_base( );
		if ( !base ) {
			return;
		}


		// fix movement for ag2
		diag::set_exception_phase( "input apply: subtick movement" );
		if (!this->has_analog_subticks(base)) {
			if (const auto step = systems::g_input.acquire_subtick_step (base->mutable_subtick_moves ())) {

				const auto movement_services = local.pawn ? memory::read<std::uintptr_t> (local.pawn + SCHEMA ("C_BasePlayerPawn", "m_pMovementServices"_hash)) : 0;
				if (movement_services) {
					step->set_button (0);
					step->set_pressed (false);
					step->set_when (0.0f);
					step->set_analog_forward_delta (base->forwardmove () - memory::read<float> (movement_services + SCHEMA ("CPlayer_MovementServices", "m_flCmdForwardMove"_hash)));
					step->set_analog_left_delta (base->leftmove () - memory::read<float> (movement_services + SCHEMA ("CPlayer_MovementServices", "m_flCmdLeftMove"_hash)));
				}
			}
		}

		// A later feature may append an event at time zero. Serialize only after
		// validating the protobuf representation; never sort through a stale rep.
		(void) sort_subtick_moves_safely( base->mutable_subtick_moves( ) );

		diag::set_exception_phase( "input apply: buttons" );
		auto buttons = const_cast<proto::in_button_state_pb*>( base->buttons_pb( ) );
		if ( !buttons )
		{
			const auto raw_base =
				reinterpret_cast<std::uintptr_t>( base ) - proto::message_impl_offset;
			const auto arena_bits = memory::read<std::uintptr_t>( raw_base + 0x08 );
			auto arena = arena_bits & ~0x3ull;
			if ( arena_bits & 1 )
			{
				arena = memory::read<std::uintptr_t>( arena );
			}

			const auto raw_buttons =
				memory::call<void*>( PATTERN( patterns::button_state_alloc ), arena );
			if ( raw_buttons )
			{
				base->m_buttons_pb =
					reinterpret_cast<proto::in_button_state_pb*>( raw_buttons );
				buttons = proto::impl_ptr<proto::in_button_state_pb>( raw_buttons );
			}
		}

		if ( buttons )
		{
			base->m_has_bits.set( 0x2u );
			buttons->set_buttonstate1( this->m_current_cmd->buttons.value );
			buttons->set_buttonstate2( this->m_current_cmd->buttons.value_changed );
			buttons->set_buttonstate3( this->m_current_cmd->buttons.value_scroll );
		}

		diag::set_exception_phase( "input apply: crc" );
		this->calculate_crc( base );
	}

	input::usercmd* input::get_current_cmd( std::uintptr_t local_controller ) const
	{
		const auto get_usercmd_base = PATTERN (patterns::get_usercmd_base);
		const auto get_usercmd = PATTERN (patterns::get_usercmd);
		if ( !get_usercmd_base || !get_usercmd )
		{
			return nullptr;
		}

		const auto usercmd_base = memory::call<std::uintptr_t>( get_usercmd_base, local_controller );
		if ( !usercmd_base )
		{
			return nullptr;
		}

		const auto sequence = memory::safe_read<int>( usercmd_base + 0x5910 );
		if ( !sequence )
		{
			return nullptr;
		}

		return memory::call<usercmd*>( get_usercmd, local_controller, *sequence );
	}

	proto::subtick_move_step* input::acquire_subtick_step( proto::repeated_ptr_field<proto::subtick_move_step>* subtick_moves ) const
	{
		if ( !subtick_moves )
		{
			return nullptr;
		}

		proto::repeated_ptr_field<proto::subtick_move_step>::rep_t* rep{};
		int current_size{};
		int allocated_size{};
		if ( !read_subtick_rep( subtick_moves, rep, current_size, allocated_size ) ||
			current_size >= k_max_subtick_moves )
			return nullptr;

		if ( rep )
		{
			if ( current_size < allocated_size )
			{
				const auto element = read_subtick_element( rep, current_size );
				proto::subtick_move_step* step{};
				if ( reset_subtick_step( element, step ) )
				{
					if ( !valid_runtime_pointer( reinterpret_cast<std::uintptr_t>( step ) ) )
						return nullptr;
					subtick_moves->m_current_size = current_size + 1;
					return step;
				}
			}
		}

		const auto allocator = PATTERN (patterns::subtick_move_alloc);
		const auto push = PATTERN (patterns::utl_vector_push);
		if ( !allocator || !push )
			return nullptr;

		const auto arena = memory::safe_read<void*>( reinterpret_cast<std::uintptr_t>( subtick_moves ) +
			offsetof( proto::repeated_ptr_field<proto::subtick_move_step>, m_arena ) ).value_or( nullptr );
		const auto move_step = memory::safe_call<void*>( allocator, arena );
		if ( !valid_runtime_pointer( reinterpret_cast<std::uintptr_t>( move_step ) ) )
			return nullptr;

		const auto field_address = reinterpret_cast<std::uintptr_t>( subtick_moves );
		const auto before_size = memory::safe_read<int>( field_address +
			offsetof( proto::repeated_ptr_field<proto::subtick_move_step>, m_current_size ) ).value_or( -1 );
		if ( before_size < 0 || before_size >= k_max_subtick_moves )
			return nullptr;

		(void) memory::safe_call<std::uintptr_t>( push, field_address,
			reinterpret_cast<std::uintptr_t>( move_step ) );

		const auto after_size = memory::safe_read<int>( field_address +
			offsetof( proto::repeated_ptr_field<proto::subtick_move_step>, m_current_size ) ).value_or( -1 );
		const auto after_rep = memory::safe_read<proto::repeated_ptr_field<proto::subtick_move_step>::rep_t*>(
			field_address + offsetof( proto::repeated_ptr_field<proto::subtick_move_step>, m_rep ) ).value_or( nullptr );
		if ( after_size != before_size + 1 || after_size > k_max_subtick_moves || !after_rep )
			return nullptr;

		const auto stored = read_subtick_element( after_rep, before_size );
		if ( stored != move_step )
			return nullptr;

		if ( auto* const step = proto::impl_ptr<proto::subtick_move_step>( move_step ) )
		{
			if ( !valid_runtime_pointer( reinterpret_cast<std::uintptr_t>( step ) ) )
				return nullptr;
			return step;
		}

		return nullptr;
	}

	math::vector3 input::get_view_angles( ) const
	{
		const auto fn = PATTERN( patterns::get_view_angles );
		if ( !fn || !addresses::globals::csgo_input )
		{
			return {};
		}

		const auto ptr = memory::call<math::vector3*>( fn, addresses::globals::csgo_input, 0 );
		if ( !ptr )
		{
			return {};
		}

		return memory::safe_read<math::vector3>( reinterpret_cast<std::uintptr_t>( ptr ) ).value_or( math::vector3{} );
	}

	proto::input_history_entry* input::push_input_history( usercmd* cmd, const input_history_params& params ) const
	{
		auto history_field = cmd->csgo_user_cmd.mutable_input_history( );
		if ( !history_field )
		{
			return nullptr;
		}

		proto::input_history_entry* entry{ nullptr };

		if ( history_field->m_rep )
		{
			if ( history_field->m_current_size < history_field->m_rep->allocated_size )
			{
				auto element = history_field->m_rep->elements[ history_field->m_current_size ];
				if ( element )
				{
					history_field->m_current_size++;
					entry = proto::impl_ptr<proto::input_history_entry>( element );
				}
			}
		}

		if ( !entry )
		{
			auto raw = memory::call<void*>(PATTERN (patterns::history_field_alloc), history_field->m_arena );
			if ( !raw )
			{
				return nullptr;
			}

			memory::call<std::uintptr_t>(PATTERN (patterns::utl_vector_push), reinterpret_cast< std::uintptr_t >( history_field ), reinterpret_cast< std::uintptr_t >( raw ) );

			entry = proto::impl_ptr<proto::input_history_entry>( raw );
		}

		if ( !entry )
		{
			return nullptr;
		}

		entry->set_render_tick_count( params.render_tick );
		entry->set_render_tick_fraction( params.render_frac );
		entry->set_player_tick_count( params.player_tick );
		entry->set_player_tick_fraction( params.player_frac );
		entry->set_frame_number( params.frame_number );
		entry->set_target_ent_index( params.target_ent_index );

		if ( auto va = entry->mutable_view_angles( ) )
		{
			va->set_x( params.view_angles.x );
			va->set_y( params.view_angles.y );
			va->set_z( params.view_angles.z );
		}

		if ( auto sp = entry->mutable_shoot_position( ) )
		{
			sp->set_x( params.shoot_position.x );
			sp->set_y( params.shoot_position.y );
			sp->set_z( params.shoot_position.z );
		}

		if ( auto ci = entry->mutable_cl_interp( ) )
		{
			ci->set_frac( params.cl_interp_frac );
		}

		if ( auto si0 = entry->mutable_sv_interp0( ) )
		{
			si0->set_src_tick( params.sv_interp0_src );
			si0->set_dst_tick( params.sv_interp0_dst );
			si0->set_frac( params.sv_interp0_frac );
		}

		if ( auto si1 = entry->mutable_sv_interp1( ) )
		{
			si1->set_src_tick( params.sv_interp1_src );
			si1->set_dst_tick( params.sv_interp1_dst );
			si1->set_frac( params.sv_interp1_frac );
		}

		if ( auto pi = entry->mutable_player_interp( ) )
		{
			pi->set_src_tick( params.player_interp_src );
			pi->set_dst_tick( params.player_interp_dst );
			pi->set_frac( params.player_interp_frac );
		}

		if ( params.fill_cheat_check_data )
		{
			if ( auto head = entry->mutable_target_head_pos_check( ) )
			{
				head->set_x( params.target_head_pos.x );
				head->set_y( params.target_head_pos.y );
				head->set_z( params.target_head_pos.z );
			}

			if ( auto abs_pos = entry->mutable_target_abs_pos_check( ) )
			{
				abs_pos->set_x( params.target_abs_pos.x );
				abs_pos->set_y( params.target_abs_pos.y );
				abs_pos->set_z( params.target_abs_pos.z );
			}

			if ( auto abs_ang = entry->mutable_target_abs_ang_check( ) )
			{
				abs_ang->set_x( params.target_abs_ang.x );
				abs_ang->set_y( params.target_abs_ang.y );
				abs_ang->set_z( params.target_abs_ang.z );
			}
		}

		return entry;
	}

	void input::set_view_angles( const math::vector3& angles ) const
	{
		const auto fn = PATTERN( patterns::set_view_angles );
		if ( !fn || !addresses::globals::csgo_input )
		{
			return;
		}

		memory::call<void>( fn, addresses::globals::csgo_input, 0, &angles );
	}

	void input::desubtick( usercmd* cmd ) const
	{
		cmd->csgo_user_cmd.mutable_base( )->mutable_subtick_moves( )->clear( );
	}

	void input::set_weapon_select( usercmd* cmd, std::uintptr_t csgo_input ) const
	{
		const auto frame_data = csgo_input + 552;
		const auto weapon_handle = memory::read<uint32_t>( frame_data + 1132 );

		if ( !weapon_handle )
		{
			return;
		}

		const auto weapon = systems::g_entities.lookup( weapon_handle );
		if ( !weapon )
		{
			return;
		}

		auto entity_index{ -1 };
		memory::call<int*>(PATTERN (patterns::weapon_get_entity_index), weapon, &entity_index );

		if ( entity_index == -1 )
		{
			return;
		}

		const auto base = cmd->csgo_user_cmd.mutable_base( );
		if ( base )
		{
			base->set_weaponselect( entity_index );
		}
	}

	input::usercmd* input::get_command_by_sequence( std::uintptr_t local_controller, int sequence ) const
	{
		const auto get_usercmd_base = PATTERN (patterns::get_usercmd_base);
		if ( !get_usercmd_base )
		{
			return nullptr;
		}

		const auto usercmd_base = memory::call<std::uintptr_t>( get_usercmd_base, local_controller );
		if ( !usercmd_base )
		{
			return nullptr;
		}

		const auto index = sequence % 150;
		return reinterpret_cast< usercmd* >( usercmd_base + sizeof( usercmd ) * index );
	}

	bool input::is_subtick_overwrite( usercmd* cmd ) const
	{
		return memory::read<int>( reinterpret_cast< std::uintptr_t >( cmd ) + 148 ) == 2;
	}

	bool input::calculate_crc( proto::base_usercmd_pb* base ) const
	{
		if ( !base )
		{
			return false;
		}

		const auto string_copy = PATTERN( patterns::string_copy );
		const auto serialize_move_crc = PATTERN( patterns::serialize_move_crc );
		if ( !string_copy || !serialize_move_crc )
		{
			diag::write( diag::level::error, "input CRC helpers unavailable; capturing a pre-termination diagnostic dump" );
			diag::capture_snapshot( "input CRC helpers unavailable" );
			return false;
		}

		const auto btns = base->buttons_pb( );
		const auto va = base->viewangles( );

		std::uint8_t buf[ 64 ]{};
		std::uint8_t btn_size{ 0 };

		auto p = buf;

		auto bs1 = btns ? btns->buttonstate1( ) : 0;
		auto bs2 = btns ? btns->buttonstate2( ) : 0;
		auto bs3 = btns ? btns->buttonstate3( ) : 0;

		if ( bs1 ) { btn_size += 9; }
		if ( bs2 ) { btn_size += 9; }
		if ( bs3 ) { btn_size += 9; }

		if ( btn_size > 0 )
		{
			*p++ = 0x1a;
			*p++ = btn_size;

			if ( bs1 ) { *p++ = 0x09; std::memcpy( p, &bs1, 8 ); p += 8; }
			if ( bs2 ) { *p++ = 0x11; std::memcpy( p, &bs2, 8 ); p += 8; }
			if ( bs3 ) { *p++ = 0x19; std::memcpy( p, &bs3, 8 ); p += 8; }
		}

		auto pitch = va ? va->x( ) : 0.0f;
		auto yaw = va ? va->y( ) : 0.0f;
		auto roll = va ? va->z( ) : 0.0f;

		std::uint8_t va_size{ 0 };
		if ( pitch != 0.0f ) { va_size += 5; }
		if ( yaw != 0.0f ) { va_size += 5; }
		if ( roll != 0.0f ) { va_size += 5; }

		if ( va_size > 0 )
		{
			*p++ = 0x22;
			*p++ = va_size;

			if ( pitch != 0.0f ) { *p++ = 0x0d; std::memcpy( p, &pitch, 4 ); p += 4; }
			if ( yaw != 0.0f ) { *p++ = 0x15; std::memcpy( p, &yaw, 4 ); p += 4; }
			if ( roll != 0.0f ) { *p++ = 0x1d; std::memcpy( p, &roll, 4 ); p += 4; }
		}

		auto total = static_cast< int >( p - buf );

		base->m_has_bits.set( 0x1u );

		auto raw_msg = reinterpret_cast< std::uintptr_t >( base ) - proto::message_impl_offset;
		auto arena_raw = memory::read<std::uintptr_t>( raw_msg + 0x08 );
		auto arena = arena_raw & ~0x3ull;

		if ( arena_raw & 1 )
		{
			arena = memory::read<std::uintptr_t>( arena );
		}

		std::uint8_t msg[ 0x18 ]{};
		memory::call<void>( string_copy, reinterpret_cast< std::uintptr_t >( msg ), reinterpret_cast< std::uintptr_t >( buf ), total );
		memory::call<void>( serialize_move_crc, &base->m_move_crc, reinterpret_cast< std::uintptr_t >( msg ), arena );

		return true;
	}

} // namespace systems
