#pragma once

#include <Windows.h>
#include <atomic>
#include <new>

namespace utilities::tls {

        namespace detail {

                struct local_node
                {
                        void* instance{};
                        void ( *shutdown )( void* ) noexcept{};
                        local_node* next{};
                };

                inline SRWLOCK g_registry_lock = SRWLOCK_INIT;
                inline local_node* g_registry_head{};

                inline void register_local( local_node& node ) noexcept
                {
                        AcquireSRWLockExclusive( &g_registry_lock );
                        node.next = g_registry_head;
                        g_registry_head = &node;
                        ReleaseSRWLockExclusive( &g_registry_lock );
                }

                inline void unregister_local( local_node& node ) noexcept
                {
                        AcquireSRWLockExclusive( &g_registry_lock );
                        auto** current = &g_registry_head;
                        while ( *current && *current != &node )
                                current = &( *current )->next;
                        if ( *current )
                                *current = node.next;
                        node.next = nullptr;
                        ReleaseSRWLockExclusive( &g_registry_lock );
                }

                inline void shutdown_all_locals( ) noexcept
                {
                        AcquireSRWLockExclusive( &g_registry_lock );
                        for ( auto* current = g_registry_head; current; current = current->next )
                                current->shutdown( current->instance );
                        ReleaseSRWLockExclusive( &g_registry_lock );
                }

        } // namespace detail

        // Dynamic TLS wrapper for manual-mapped modules.
        // Static TLS (__declspec(thread) / thread_local) requires the Windows loader
        // to process the .tls section, which does not happen with manual mapping.
        // This wrapper uses TlsAlloc/TlsGetValue/TlsSetValue which work for any thread.

        template <typename T>
        class slot
        {
        public:
                slot( ) noexcept : m_index( TLS_OUT_OF_INDEXES ) { }

                ~slot( ) noexcept
                {
                        if ( m_index != TLS_OUT_OF_INDEXES )
                        {
                                TlsFree( m_index );
                        }
                }

                slot( const slot& ) = delete;
                slot& operator=( const slot& ) = delete;

                slot( slot&& other ) noexcept : m_index( other.m_index )
                {
                        other.m_index = TLS_OUT_OF_INDEXES;
                }

                slot& operator=( slot&& other ) noexcept
                {
                        if ( this != &other )
                        {
                                if ( m_index != TLS_OUT_OF_INDEXES )
                                {
                                        TlsFree( m_index );
                                }
                                m_index = other.m_index;
                                other.m_index = TLS_OUT_OF_INDEXES;
                        }
                        return *this;
                }

                [[nodiscard]] DWORD ensure( ) noexcept
                {
                        if ( m_index == TLS_OUT_OF_INDEXES )
                        {
                                const DWORD idx = TlsAlloc( );
                                if ( idx != TLS_OUT_OF_INDEXES )
                                {
                                        if ( InterlockedCompareExchange( (volatile LONG*)&m_index, (LONG)idx, (LONG)TLS_OUT_OF_INDEXES ) != (LONG)TLS_OUT_OF_INDEXES )
                                        {
                                                TlsFree( idx );
                                        }
                                }
                        }
                        return m_index;
                }

                [[nodiscard]] bool is_valid( ) const noexcept
                {
                        return m_index != TLS_OUT_OF_INDEXES;
                }

                [[nodiscard]] T* get( ) noexcept
                {
                        if ( m_index == TLS_OUT_OF_INDEXES )
                        {
                                return nullptr;
                        }
                        return static_cast<T*>( TlsGetValue( m_index ) );
                }

                [[nodiscard]] const T* get( ) const noexcept
                {
                        if ( m_index == TLS_OUT_OF_INDEXES )
                        {
                                return nullptr;
                        }
                        return static_cast<const T*>( TlsGetValue( m_index ) );
                }

                void set( T* value ) noexcept
                {
                        if ( m_index != TLS_OUT_OF_INDEXES )
                        {
                                TlsSetValue( m_index, value );
                        }
                }

        private:
                DWORD m_index;
        };

        // Per-thread C++ objects for manually mapped modules. Unlike the
        // language's thread_local storage, FLS does not require the loader to
        // allocate and initialize the module's static TLS block.
        template <typename T>
        class local
        {
        public:
                local( ) noexcept
                {
                        m_node = { this, shutdown_registered, nullptr };
                        detail::register_local( m_node );
                }

                ~local( ) noexcept
                {
                        shutdown( );
                        detail::unregister_local( m_node );
                }

                local( const local& ) = delete;
                local& operator=( const local& ) = delete;

                [[nodiscard]] T* get( ) noexcept
                {
                        const DWORD index = ensure( );
                        if ( index == FLS_OUT_OF_INDEXES )
                                return nullptr;

                        if ( auto* value = static_cast<T*>( FlsGetValue( index ) ) )
                                return value;

                        T* value{};
                        try
                        {
                                value = new T{};
                        }
                        catch ( ... )
                        {
                                return nullptr;
                        }

                        if ( !FlsSetValue( index, value ) )
                        {
                                delete value;
                                return nullptr;
                        }

                        return value;
                }

                void shutdown( ) noexcept
                {
                        const DWORD index = m_index.exchange(
                                FLS_OUT_OF_INDEXES, std::memory_order_acq_rel );
                        if ( index == FLS_OUT_OF_INDEXES )
                                return;

                        if ( auto* value = static_cast<T*>( FlsGetValue( index ) ) )
                        {
                                FlsSetValue( index, nullptr );
                                delete value;
                        }

                        // Clear the callback before the manually mapped code is
                        // released. Values owned by other live threads are left
                        // for process teardown rather than calling into unloaded code.
                        FlsFree( index );
                }

        private:
                static void shutdown_registered( void* instance ) noexcept
                {
                        static_cast<local*>( instance )->shutdown( );
                }

                static VOID CALLBACK destroy_value( PVOID value ) noexcept
                {
                        delete static_cast<T*>( value );
                }

                [[nodiscard]] DWORD ensure( ) noexcept
                {
                        DWORD index = m_index.load( std::memory_order_acquire );
                        if ( index != FLS_OUT_OF_INDEXES )
                                return index;

                        const DWORD allocated = FlsAlloc( destroy_value );
                        if ( allocated == FLS_OUT_OF_INDEXES )
                                return FLS_OUT_OF_INDEXES;

                        DWORD expected = FLS_OUT_OF_INDEXES;
                        if ( !m_index.compare_exchange_strong(
                                expected, allocated,
                                std::memory_order_release,
                                std::memory_order_acquire ) )
                        {
                                FlsFree( allocated );
                        }

                        return m_index.load( std::memory_order_acquire );
                }

                std::atomic<DWORD> m_index{ FLS_OUT_OF_INDEXES };
                detail::local_node m_node{};
        };

        inline void shutdown_all( ) noexcept
        {
                detail::shutdown_all_locals( );
        }

} // namespace utilities::tls
