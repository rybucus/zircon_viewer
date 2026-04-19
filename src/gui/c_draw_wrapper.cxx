#include <gui/c_draw_wrapper.hxx>
#include <gui/window_impl_t.hxx>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

namespace zircon::gui
{

void c_draw_wrapper::init( window_impl_t* window )
{
    IMGUI_CHECKVERSION( );
    ImGui::CreateContext( ) ;

    ImGuiIO& io = ImGui::GetIO( );
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.IniFilename = nullptr;

    ImGuiStyle& style = ImGui::GetStyle( );
    style.ScaleAllSizes( window->m_scale );

    ImGui_ImplGlfw_InitForOpenGL( window->m_window, true );
    ImGui_ImplOpenGL3_Init( "#version 330" );
}

void c_draw_wrapper::begin_frame( window_impl_t* window )
{
    ImGui_ImplOpenGL3_NewFrame( );
    ImGui_ImplGlfw_NewFrame( );
    ImGui::NewFrame( );
}

void c_draw_wrapper::end_frame( window_impl_t* window )
{
    ImGui::Render( );
    ImVec4 clear_color = ImVec4( 0.f, 0.f, 0.f, 1.f );
    int width{ }, heigth{ };
    glfwGetFramebufferSize( window->m_window, &width, &heigth );
    glViewport( 0, 0, width, heigth );
    glClearColor( clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w );
    glClear( GL_COLOR_BUFFER_BIT );
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData( ) );
    glfwSwapBuffers( window->m_window );
}

void c_draw_wrapper::cleanup( )
{
    ImGui_ImplOpenGL3_Shutdown( );
    ImGui_ImplGlfw_Shutdown( );
    ImGui::DestroyContext( );
}

float c_draw_wrapper::get_delta_time( ) const
{
    return ImGui::GetIO( ).DeltaTime;
}

} // namespace zircon::gui
