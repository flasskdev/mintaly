#pragma once
#include "jumpbug_cycle.hpp"

namespace features::movement {

	// A failed guarded read can return the canonical -1 sentinel from an
	// engine field. Treat it exactly like null before using a value as a
	// pointer or as the base of another memory read.
	[[nodiscard]] inline bool valid_runtime_address( std::uintptr_t address ) noexcept
	{
		return address >= 0x10000ull &&
			address != ( std::numeric_limits<std::uintptr_t>::max )( ) &&
			address <= 0x00007FFFFFFFFFFFull;
	}

	class bhop
	{
	public:
		void on_create_move( systems::input::usercmd* cmd ) const;

		[[nodiscard]] bool landing_ok_this_tick( ) const { return this->m_landing_ok; }

	private:
		mutable bool m_landing_ok{ false };
	};

	class fastladder
	{
	public:
		void on_create_move( systems::input::usercmd* cmd ) const;
	};

	class edgejump
	{
	public:
		void on_create_move( systems::input::usercmd* cmd ) const;
	};

	class quickstop
	{
	public:
		void on_create_move( systems::input::usercmd* cmd ) const;
	};

	class jumpbug
	{
	public:
		void on_create_move( systems::input::usercmd* cmd, std::uint64_t original_buttons );

		[[nodiscard]] bool active_this_tick( ) const { return this->m_active_this_tick; }

	private:
		bool m_active_this_tick{ false };
		bool m_owned_duck{ false };
		bool m_fired_last_tick{ false };
		std::uintptr_t m_pawn{};
		jumpbug_cycle m_cycle{};
	};

	class slowwalk
	{
	public:
		void on_create_move( systems::input::usercmd* cmd );

		[[nodiscard]] bool active_this_tick( ) const { return this->m_active_this_tick; }

	private:
		bool m_active_this_tick{ false };
	};

	class test_strafer
	{
	public:
		void on_create_move( systems::input::usercmd* cmd, std::uint64_t original_buttons );
		[[nodiscard]] bool is_active( ) const;
		[[nodiscard]] bool handled_this_tick( ) const { return this->m_handled_this_tick; }

	private:
		[[nodiscard]] bool emit_step( proto::base_usercmd_pb* base, float when, float forward_delta, float left_delta ) const;
		void check_button( std::uintptr_t current_buttons, std::uintptr_t button );

		std::uintptr_t m_last_buttons{};
		std::uintptr_t m_last_pressed{};
		bool m_side_toggle{};
		bool m_handled_this_tick{};
	};

} // namespace features::movement
