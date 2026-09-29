#pragma once

namespace hooking {

	class jmp
	{
	public:
		bool create( void* target, void* hook_function );
		bool enable( );
		bool disable( );
		void reset( );

		template <typename T, typename... args_t>
		T call( args_t... args ) const
		{
			if ( !this->m_trampoline ) [[unlikely]]
			{
				if constexpr (std::is_same_v<T, void>)
					return;
				else
					return T {};
			}

			return reinterpret_cast< T( * )( args_t... ) >( this->m_trampoline )( args... );
		}

		// SEH-safe variant: the original Present may fault on a half-released swap
		// chain during level transitions. Calling through the trampoline under
		// __try keeps that fault from taking the process down. Must not be used
		// from a function that holds non-trivial destructibles (MSVC C2712).
		template <typename T, typename... args_t>
		T safe_call( args_t... args ) const
		{
			if ( !this->m_trampoline )
			{
				if constexpr (std::is_same_v<T, void>)
					return;
				else
					return T {};
			}

			__try
			{
				return reinterpret_cast< T( * )( args_t... ) >( this->m_trampoline )( args... );
			}
			__except ( EXCEPTION_EXECUTE_HANDLER )
			{
				if constexpr (std::is_same_v<T, void>)
					return;
				else
					return T {};
			}
		}

		template <typename T>
		T original( ) const
		{
			return reinterpret_cast< T >( this->m_trampoline );
		}

		explicit operator bool( ) const { return this->is_valid( ); }
		bool is_valid( ) const { return this->m_target && this->m_trampoline; }
		bool is_enabled( ) const { return this->m_enabled; }
		void* get_target( ) const { return this->m_target; }
		void* get_trampoline( ) const { return this->m_trampoline; }
		const std::uint8_t* get_original_bytes( ) const { return this->m_original_bytes; }
		std::size_t get_original_length( ) const { return this->m_original_length; }

	private:
		void* m_target{};
		void* m_hook{};
		void* m_trampoline{};
		std::uint8_t m_original_bytes[ 32 ]{};
		std::uint8_t m_hook_bytes[ 16 ]{};
		std::size_t m_original_length{};
		std::size_t m_patch_size{};
		bool m_enabled{};
	};

	namespace allocator {

		void* allocate( std::size_t size, void* near_ );
		void free( void* address );

	} // namespace allocator

	namespace manager {

		struct entry
		{
			jmp* hook;
			void* detour;
			const char* name;
			std::uintptr_t address;
		};

		bool create( const std::initializer_list<entry>& entries );

	} // namespace manager

} // namespace hooking
