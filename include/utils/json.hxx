#pragma once

#include <string>

#include <nlohmann/json.hpp>

namespace zircon::utils
{

using json_t = nlohmann::json;

inline std::string jsonize_key( const std::string_view key )
{
    return std::string{ key } + ".json";
}

template< typename T >
inline T get_value( const json_t& json, const std::string_view key, const T& fallback = { } ) noexcept
{
    if( json.is_null( ) || json.empty( ) || !json.contains( key ) )
    {
        return fallback;
    }
    return json.at( key ).get< T >( );
}

} // namespace zircon::utils
