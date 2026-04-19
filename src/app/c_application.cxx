#include <thread>
#include <chrono>

#include <app/c_application.hxx>
#include <app/application_config.hxx>
#include <app/i_module.hxx>

#include <gui/c_window.hxx>
#include <gui/c_gui.hxx>

#include <utils/files.hxx>
#include <utils/logging.hxx>

namespace zircon::app
{

c_application::c_application( )
{
    m_draw = std::make_unique< gui::c_draw_wrapper >( );
}

void c_application::add_module( const std::string_view key, i_module* module )
{
    m_modules.try_emplace( std::string{ key }, module );

    module->on_attach( this );
}

void c_application::remove_module( const std::string_view key )
{
    auto it = m_modules.find( std::string{ key } );
    if ( it != m_modules.end( ) )
    {
        const auto module{ std::get< i_module* >( *it ) };

        module->on_detach( this );

        m_modules.erase( it );
    }
}

bool c_application::on_load( const std::set< std::string >& args )
{
    if ( args.contains( "-h" ) )
    {
        print_help_message( );
        return false;
    }

    add_module( gui::window_preset_t::k_key, &gui::g_window_settings );
    add_module( gui::c_gui::k_key, &gui::g_gui );

    if ( !args.contains( "-p" ) )
    {
        load_configs( );
    }

    return true;
}

void c_application::on_unload( )
{
    for_each_module( [ this ] ( const std::string& key, i_module* module ) {
        module->on_detach( this );
    } );
}

void c_application::on_load_config( const utils::json_t& config )
{

}

void c_application::run( )
{
    gui::c_window window{ };

    m_draw->init( window.get_impl( ) );

    notify_frame_stage( { e_frame_stage::k_init } );

    while ( !window.is_should_close( ) )
    {
        window.catch_events( );

        notify_frame_stage( { e_frame_stage::k_input_handled } );

        if ( window.is_unfocused( ) )
        {
            std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
            continue;
        }

        m_draw->begin_frame( window.get_impl( ) );
        {
            window_params_t window_params{ window.get_window_width( ), window.get_window_heigth( ),
                window.get_window_scale( ), m_draw->get_delta_time( )
            };

            notify_frame_stage( { e_frame_stage::k_stage_begin, window_params } );
            {

            }
            notify_frame_stage( { e_frame_stage::k_stage_end, window_params } );
        }
        m_draw->end_frame( window.get_impl( ) );
    }

    on_unload( );

    m_draw->cleanup( );
}

void c_application::notify_frame_stage( const frame_state_t& frame )
{
    for_each_module( [ frame ] ( const std::string& key, i_module* module ) {
        module->on_frame_stage_notify( frame );
    } );
}

void c_application::for_each_module( c_application::module_callback_t&& callback )
{
    std::ranges::for_each( m_modules, [ &callback ] ( const auto& entry ) {
        const auto& [ key, value ] = entry;
        callback( key, value );
    } );
}

i_module* c_application::get_module( const std::string_view key )
{
    auto it = m_modules.find( std::string{ key } );
    if ( it != m_modules.end( ) )
    {
        return std::get< i_module* >( *it );
    }
    return nullptr;
}

void c_application::load_configs( )
{
    on_load_config( utils::load_json_file( configs_dir( ) + utils::jsonize_key( k_key ), k_application_config.dump( 4 ) ) );

    for_each_module( [ configs = configs_dir( ) ] ( const std::string& key, i_module* module ) {
        utils::println( "loading config for module [{}]", key );
        module->on_config_load( utils::load_json_file( configs + utils::jsonize_key( key ) ) );
    } );
}

void c_application::print_help_message( ) const
{
    utils::println( "use -h for help message\n"
                    "use -p for disable configs load" );
}

std::string c_application::configs_dir( ) const
{
    return utils::get_current_dir( ) + "/configs/";
}

} // namespace zircon::app
