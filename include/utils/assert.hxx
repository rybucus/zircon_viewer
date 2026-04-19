#pragma once

#include <source_location>
#include <string_view>
#include <utils/logging.hxx>

namespace zircon::utils
{

[[noreturn]] inline void assert_fail( const std::string_view message, const std::source_location& location = std::source_location::current( ) ) noexcept
{
    zircon::utils::println( "[ASSERT FAILED {} at {} line]  {}",
        location.file_name( ), location.line( ), message.empty( ) ? "<no message>" : message
    );

    abort( );
}

} // namespace zircon::utils

#ifndef NDEBUG
#   define ZIRCON_ASSERT( expr, msg ) ( ( expr ) ? static_cast< void >( 0 ) : ::zircon::utils::assert_fail( ( msg ), std::source_location::current( ) ) )
#   define ZIRCON_ASSERT_NOMSG( expr ) ( ( expr ) ? static_cast< void >( 0 ) : ::zircon::utils::assert_fail( std::string_view{ }, std::source_location::current( ) ) )
#else
#   define ZIRCON_ASSERT( expr, msg ) static_cast< void >( 0 )
#endif
