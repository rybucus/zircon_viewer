#pragma once

#include <cstdint>
#include <limits>
#include <string>
#include <string_view>
#include <optional>
#include <memory>

namespace zircon::native
{

struct process_t
{
public:
    using pid_t = std::int32_t;

public:
    enum : pid_t { k_invalid_pid = std::numeric_limits< pid_t >::max( ) };

public:
    std::string m_name{ };
    pid_t m_pid{ k_invalid_pid };

};

class i_core
{
public:
    virtual ~i_core( ) = default;

public:
    virtual std::vector< process_t > get_all_processes( ) const = 0;
    virtual std::optional< process_t > get_process( const std::string_view process_name ) const = 0;

};

// extern std::unique_ptr< i_core > g_core;

} // namespace zircon::native
