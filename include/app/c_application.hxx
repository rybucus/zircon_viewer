#pragma once

#include <unordered_map>

#include <app/i_application.hxx>

#include <gui/c_draw_wrapper.hxx>

namespace zircon::app
{

class i_module;

class c_application : public i_application
{
public:
    static constexpr std::string_view k_key = "application";

public:
    c_application( );

public:
    void add_module( const std::string_view key, i_module* module ) override;
    void remove_module( const std::string_view key ) override;
    bool on_load( const std::set< std::string >& args ) override;
    void on_unload( ) override;
    void on_load_config( const utils::json_t& config ) override;
    void run( ) override;
    void notify_frame_stage( const frame_state_t& frame ) override;

public:
    void for_each_module( module_callback_t&& callback ) override;
    i_module* get_module( const std::string_view key ) override;

public:
    void load_configs( );

private:
    std::string configs_dir( ) const;
    void print_help_message( ) const;

private:
    std::unordered_map< std::string, i_module* > m_modules{ };
    std::unique_ptr< gui::c_draw_wrapper > m_draw{ };

};

} // namespace zircon::app
