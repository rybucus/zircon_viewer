#include <gui/c_window.hxx>
#include <gui/window_impl_t.hxx>

#include <utils/assert.hxx>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

namespace zircon::gui
{

void window_preset_t::on_config_load( const utils::json_t& config )
{
    if ( !config.empty( ) )
    {
        m_width = utils::get_value( config, "width", k_default_width );
        m_heigth = utils::get_value( config, "heigth", k_default_heigth );
        m_name = utils::get_value( config, "name", k_default_name );
        m_should_use_vsync = utils::get_value( config, "vsync", k_vsync_default );
        m_override_scale = utils::get_value( config, "scale", k_scale_default );
    }
}

c_window::c_window( )
{
    m_impl = std::make_unique< window_impl_t >( );

    ZIRCON_ASSERT( glfwInit( ), "FAILED TO INIT GLFW" );

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    m_impl->m_scale = g_window_settings.m_override_scale != -1.f ? g_window_settings.m_override_scale : ImGui_ImplGlfw_GetContentScaleForMonitor( glfwGetPrimaryMonitor( ) );
    m_impl->m_window = glfwCreateWindow( g_window_settings.m_width,
        g_window_settings.m_heigth, g_window_settings.m_name.c_str( ), nullptr, nullptr
    );

    ZIRCON_ASSERT( m_impl->m_window, "FAILED TO CREATE WINDOW" );

    glfwMakeContextCurrent( m_impl->m_window );
    g_window_settings.m_should_use_vsync ? glfwSwapInterval( 1 ) : ( void )( 0 );
}

void c_window::catch_events( )
{
    glfwPollEvents( );
}

c_window::~c_window( )
{
    glfwDestroyWindow( m_impl->m_window );
    glfwTerminate( );
}

bool c_window::is_should_close( ) const
{
    return glfwWindowShouldClose( m_impl->m_window );
}

bool c_window::is_unfocused( ) const
{
    return glfwGetWindowAttrib( m_impl->m_window, GLFW_ICONIFIED ) != 0;
}

window_impl_t* c_window::get_impl( ) const
{
    return m_impl.get( );
}

float c_window::get_window_width( ) const
{
    int width{ }, height{ };
    glfwGetWindowSize( m_impl->m_window, &width, &height );
    return static_cast< float >( width );
}

float c_window::get_window_heigth( ) const
{
    int width{ }, height{ };
    glfwGetWindowSize( m_impl->m_window, &width, &height );
    return static_cast< float >( height );
}

float c_window::get_window_scale( ) const
{
    return g_window_settings.m_override_scale != -1.f
        ? g_window_settings.m_override_scale
        : ImGui_ImplGlfw_GetContentScaleForMonitor( glfwGetPrimaryMonitor( ) );
}

} // namespace zircon::gui
