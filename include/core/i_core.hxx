#pragma once

#include <cstdint>
#include <limits>
#include <string>
#include <string_view>
#include <optional>
#include <memory>
#include <vector>
#include <span>

namespace zircon::native
{

using address_t = std::uintptr_t;

struct process_t
{
public:
    using pid_t = std::int32_t;

public:
    enum : pid_t { k_invalid_pid = std::numeric_limits< pid_t >::max( ) };

public:
    std::string m_name{ };
    std::string m_path{ };
    pid_t m_pid{ k_invalid_pid };

};

enum class e_memory_protection : std::uint32_t
{
    k_none    = 0,
    k_read    = 1 << 0,
    k_write   = 1 << 1,
    k_execute = 1 << 2,
    k_private = 1 << 3,
    k_shared  = 1 << 4
};

struct memory_region_t
{
public:
    address_t m_base{ };
    std::size_t m_size{ };
    e_memory_protection m_protection{ e_memory_protection::k_none };
    std::string m_name{ };
    std::string m_path{ };
};

struct module_t
{
public:
    address_t m_base{ };
    std::size_t m_size{ };
    std::string m_name{ };
    std::string m_path{ };
};

struct memory_read_result_t
{
public:
    bool m_success{ };
    std::size_t m_bytes_read{ };
    std::int32_t m_error_code{ };
};

class i_core
{
public:
    virtual ~i_core( ) = default;

public:
    virtual std::vector< process_t > get_all_processes( ) const = 0;
    virtual std::optional< process_t > get_process( const std::string_view process_name ) const = 0;

public:
    virtual bool attach_process( const process_t& process ) = 0;
    virtual void detach_process( ) = 0;
    virtual bool is_process_valid( ) const = 0;
    virtual std::optional< process_t > get_attached_process( ) const = 0;

public:
    virtual memory_read_result_t read_memory( address_t address, std::span< std::byte > buffer ) const = 0;
    virtual std::vector< memory_region_t > get_memory_regions( ) const = 0;
    virtual std::vector< module_t > get_modules( ) const = 0;
    virtual std::optional< memory_region_t > get_region( address_t address ) const = 0;

};

extern std::unique_ptr< i_core > g_core;

} // namespace zircon::native
