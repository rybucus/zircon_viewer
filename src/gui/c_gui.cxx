#include <gui/c_gui.hxx>
#include <gui/c_window.hxx>
#include <gui/verdana.hxx>

#include <imgui.h>

namespace zircon::gui
{

void c_gui::prepare_style( )
{
    auto& style = ImGui::GetStyle( );
    style.Colors[ ImGuiCol_MenuBarBg ] = ImColor{ 28, 28, 28, 255 };
    style.Colors[ ImGuiCol_WindowBg ] = ImColor{ 10, 10, 10, 255 };
    style.Colors[ ImGuiCol_Text ] = ImColor{ 235, 235, 235, 255 };
    style.Colors[ ImGuiCol_HeaderHovered ] = ImColor{ 80, 80, 80, 180 };
    style.Colors[ ImGuiCol_Header ] = ImColor{ 60, 60, 60, 180 };
    style.Colors[ ImGuiCol_PopupBg ] = ImColor{ 28, 28, 28, 255 };
    style.Colors[ ImGuiCol_Border ] = ImColor{ 60, 60, 60, 255 };

    style.FramePadding = ImVec2( style.FramePadding.x, 6.f );
    style.WindowBorderSize = 0.f;
}

void c_gui::prepare_fonts( )
{
    auto& io = ImGui::GetIO( );

    ImFontConfig font_cfg{ };
    font_cfg.PixelSnapH  = false;
    font_cfg.OversampleH = 5;
    font_cfg.OversampleV = 5;
    font_cfg.FontDataOwnedByAtlas = false;

    ImFont* verdana_font = io.Fonts->AddFontFromMemoryTTF(
        g_verdana,
        sizeof( g_verdana ),
        16.0f,
        &font_cfg,
        io.Fonts->GetGlyphRangesCyrillic( )
    );

    io.FontDefault = verdana_font;
}

void c_gui::on_config_load( const utils::json_t& config )
{

}

void c_gui::on_frame_stage_notify( const app::frame_state_t& frame )
{
    switch ( frame.m_stage )
    {
        case app::e_frame_stage::k_stage_begin:
        {
            if ( frame.m_params.has_value( ) )
            {
                on_draw_begin( frame.m_params.value( ) );
            }
            break;
        }
        case app::e_frame_stage::k_init:
        {
            prepare_fonts( );
            prepare_style( );
            break;
        }
        default:
        {
            break;
        }
    }
}

void c_gui::prepare_window( const app::window_params_t& params )
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport( );
    ImGui::SetNextWindowPos( viewport->WorkPos, ImGuiCond_Always );
    ImGui::SetNextWindowSize( viewport->WorkSize, ImGuiCond_Always );
}

void c_gui::draw_tooltip_bar( const app::window_params_t& params )
{
    if ( ImGui::BeginMenuBar( ) )
    {
        if ( ImGui::BeginMenu( "Application" ) )
        {
            if ( ImGui::MenuItem( "Exit" ) )
            {
                std::exit( 0 );
            }

#ifndef NDEBUG
            switch ( m_selected_window )
            {
                case k_main:
                {
                    if ( ImGui::MenuItem( "Enter debug" ) )
                    {
                        m_selected_window = k_debug;
                    }

                    break;
                }
                case k_debug:
                {
                    if ( ImGui::MenuItem( "Enter release" ) )
                    {
                        m_selected_window = k_main;
                    }

                    break;
                }
                default: { break; }
            }
#endif

            ImGui::EndMenu( );
        }

        if ( ImGui::BeginMenu( "Process" ) )
        {
            if ( ImGui::MenuItem( "Attach" ) )
            {

            }

            if ( ImGui::MenuItem( "Detach" ) )
            {

            }

            ImGui::EndMenu( );
        }

        ImGui::EndMenuBar( );
    }
}

void c_gui::draw_debug_window( const app::window_params_t& params )
{

}

void c_gui::on_draw_begin( const app::window_params_t& params )
{
    prepare_window( params );

    ImGui::Begin( g_window_settings.m_name.c_str( ), nullptr, get_window_flags( ) );
    {
        draw_tooltip_bar( params );

        switch ( m_selected_window )
        {
            case k_main:
            {
                break;
            }
            case k_debug:
            {
                draw_debug_window( params );
                break;
            }
            default: { break; }
        }
    }
    ImGui::End( );
}

int c_gui::get_window_flags( ) const
{
    return ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar;
}

} // namespace zircon::gui
