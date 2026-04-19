#pragma once

#include <utility>
#include <fmt/core.h>

namespace zircon::utils
{

template< typename... Args >
inline void println( fmt::format_string< Args... > format, Args&&... args ) noexcept
{
    fmt::print( format, std::forward< Args >( args )... );
    fmt::print( "\n" );
}

template< typename... Args >
inline void print( fmt::format_string< Args... > format, Args&&... args ) noexcept
{
    fmt::print( format, std::forward< Args >( args )... );
}

} // namespace zircon::utils
