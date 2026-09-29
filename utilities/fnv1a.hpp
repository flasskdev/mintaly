#pragma once

namespace fnv1a {

	constexpr std::uint32_t hash( const char* str, std::size_t length ) noexcept
	{
		std::uint32_t hash{ 2166136261u };

		for ( auto i = 0ull; i < length; ++i )
		{
			hash ^= static_cast< std::uint32_t >( str[ i ] );
			hash *= 16777619u;
		}

		return hash;
	}

	inline std::uint32_t runtime_hash( const char* str ) noexcept
	{
		if ( !str )
		{
			return 0;
		}

		__try
		{
			std::uint32_t hash{ 2166136261u };
			constexpr std::size_t max_length{ 128 };

			for ( std::size_t i = 0; i < max_length; ++i )
			{
				const auto c = str[ i ];
				if ( c == '\0' )
					return hash;

				hash ^= static_cast< std::uint32_t >( c );
				hash *= 16777619u;
			}
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
			return 0;
		}

		return 0;
	}

} // namespace fnv1a

constexpr std::uint32_t operator""_hash( const char* str, std::size_t length ) noexcept
{
	return fnv1a::hash( str, length );
}
