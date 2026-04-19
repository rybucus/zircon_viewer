#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <set>

#include <utils/json.hxx>
#include <app/frame_stage.hxx>

namespace zircon::app
{

class i_module;

class i_application
{
public:
    using module_callback_t = std::function< void( const std::string&, i_module* ) >;

public:
    virtual ~i_application( ) = default;
    virtual void add_module( const std::string_view key, i_module* module ) { }
    virtual void remove_module( const std::string_view key ) { }
    virtual bool on_load( const std::set< std::string >& args ) { return false; }
    virtual void on_unload( ) { }
    virtual void on_load_config( const utils::json_t& config ) { }
    virtual void run( ) { }
    virtual void notify_frame_stage( const frame_state_t& frame ) { }

public:
    virtual void for_each_module( module_callback_t&& callback ) { }
    virtual i_module* get_module( const std::string_view key ) { return nullptr; }

};

inline std::unique_ptr< i_application > g_application{ };

} // namespace zircon::app
