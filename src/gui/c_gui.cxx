#include <gui/c_gui.hxx>
#include <gui/c_window.hxx>
#include <gui/verdana.hxx>

#include <imgui.h>

#include <algorithm>
#include <charconv>
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <format>
#include <ranges>
#include <span>
#include <vector>

namespace zircon::gui
{

namespace
{

template< typename value_t >
bool read_value( const std::vector< std::byte >& buffer, const std::size_t offset, value_t& out )
{
    if ( offset > buffer.size( ) || buffer.size( ) - offset < sizeof( value_t ) )
    {
        return false;
    }
    std::memcpy( &out, buffer.data( ) + offset, sizeof( value_t ) );
    return true;
}

void draw_copyable_value( const std::string& value, const ImVec4& color )
{
    if ( value.empty( ) )
    {
        ImGui::TextColored( color, "\"\"" );
        return;
    }
    ImGui::TextColored( color, "%s", value.c_str( ) );
    if ( ImGui::IsItemHovered( ) && ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) )
    {
        ImGui::SetClipboardText( value.c_str( ) );
    }
    if ( ImGui::BeginPopupContextItem( "##value_copy_popup" ) )
    {
        if ( ImGui::MenuItem( "Copy value" ) )
        {
            ImGui::SetClipboardText( value.c_str( ) );
        }
        ImGui::EndPopup( );
    }
}

} // namespace

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
    style.Colors[ ImGuiCol_TableBorderLight ] = ImColor{ 100, 100, 100, 255 };
    style.Colors[ ImGuiCol_TableBorderStrong ] = ImColor{ 130, 130, 130, 255 };
    style.Colors[ ImGuiCol_HeaderActive ] = ImColor{ 80, 80, 80, 255 };
    style.Colors[ ImGuiCol_TableHeaderBg ] = ImColor{ 60, 60, 60, 255 };
    style.Colors[ ImGuiCol_Button ] = ImColor{ 60, 60, 60, 255 };
    style.Colors[ ImGuiCol_ButtonHovered ] = ImColor{ 80, 80, 80, 180 };
    style.Colors[ ImGuiCol_ButtonActive ] = ImColor{ 80, 80, 80, 255 };
    style.FramePadding = ImVec2( style.FramePadding.x, 6.f );
    style.WindowBorderSize = 0.f;
    style.ScrollbarRounding = 0.f;
    style.PopupBorderSize = 0.f;
}

void c_gui::prepare_fonts( )
{
    auto& io = ImGui::GetIO( );
    ImFontConfig font_cfg{ };
    font_cfg.PixelSnapH = false;
    font_cfg.OversampleH = 5;
    font_cfg.OversampleV = 5;
    font_cfg.FontDataOwnedByAtlas = false;
    ImFont* verdana_font = io.Fonts->AddFontFromMemoryTTF( g_verdana, sizeof( g_verdana ), 16.0f, &font_cfg, io.Fonts->GetGlyphRangesCyrillic( ) );
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
            default:
            {
                break;
            }
            }
#endif
            ImGui::EndMenu( );
        }
        if ( ImGui::BeginMenu( "Process" ) )
        {
            if ( ImGui::MenuItem( "Attach" ) )
            {
                m_open_attach_popup = true;
                m_cached_prcesses = native::g_core->get_all_processes( );
                m_selected_process = k_invalid_index;
            }
            if ( ImGui::MenuItem( "Detach" ) )
            {
                const auto old_attach = m_attached_process;
                native::g_core->detach_process( );
                m_attached_process = std::nullopt;
                hanlde_attach_process( old_attach, m_attached_process );
            }
            ImGui::EndMenu( );
        }
        ImGui::Separator( );
        const std::string process_string = std::format( "Process: {}", m_attached_process.has_value( ) ? m_attached_process->m_name : "None" );
        ImGui::TextUnformatted( process_string.c_str( ) );
        ImGui::EndMenuBar( );
    }
}

void c_gui::handle_attach_popup( )
{
    if ( m_open_attach_popup )
    {
        ImGui::OpenPopup( "Attach to process" );
        m_open_attach_popup = false;
    }
    ImGui::SetNextWindowPos( ImGui::GetMainViewport( )->GetCenter( ), ImGuiCond_Always, ImVec2( 0.5f, 0.5f ) );
    if ( ImGui::BeginPopup( "Attach to process", ImGuiWindowFlags_NoMove ) )
    {
        float name_width = 200.0f;
        float path_width = 400.0f;
        const float pid_width = 90.0f;
        for ( const auto& process : m_cached_prcesses )
        {
            name_width = std::max( name_width, ImGui::CalcTextSize( process.m_name.c_str( ) ).x + 32.0f );
            path_width = std::max( path_width, ImGui::CalcTextSize( process.m_path.c_str( ) ).x + 32.0f );
        }
        const float inner_width = pid_width + name_width + path_width;
        if ( ImGui::BeginTable( "process_table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX | ImGuiTableFlags_SizingFixedFit, ImVec2( 750.f, 400.f ), inner_width ) )
        {
            ImGui::TableSetupColumn( "PID", ImGuiTableColumnFlags_WidthFixed, pid_width );
            ImGui::TableSetupColumn( "Name", ImGuiTableColumnFlags_WidthFixed, name_width );
            ImGui::TableSetupColumn( "Path", ImGuiTableColumnFlags_WidthStretch, path_width );
            ImGui::TableHeadersRow( );
            for ( std::size_t i = 0; i < m_cached_prcesses.size( ); i++ )
            {
                const native::process_t& process = m_cached_prcesses[ i ];
                const bool is_selected = m_selected_process == i;
                const std::string selectable_id = std::to_string( process.m_pid ) + "##process";
                ImGui::TableNextRow( );
                ImGui::TableSetColumnIndex( 0 );
                ImGui::PushStyleColor( ImGuiCol_Header, ImColor{ 70, 70, 70 }.Value );
                if ( ImGui::Selectable( selectable_id.c_str( ), is_selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick ) )
                {
                    m_selected_process = i;
                    if ( ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) )
                    {
                        const auto old_attach = m_attached_process;
                        if ( native::g_core->attach_process( process ) )
                        {
                            m_attached_process = process;
                        }
                        else
                        {
                            m_attached_process = std::nullopt;
                        }
                        hanlde_attach_process( old_attach, m_attached_process );
                        ImGui::CloseCurrentPopup( );
                    }
                }
                ImGui::PopStyleColor( );
                ImGui::TableSetColumnIndex( 1 );
                ImGui::TextUnformatted( process.m_name.c_str( ) );
                ImGui::TableSetColumnIndex( 2 );
                ImGui::TextUnformatted( process.m_path.c_str( ) );
            }
            ImGui::EndTable( );
        }
        const ImVec2 size = ImGui::CalcTextSize( "Refresh" );
        const float button_width = size.x + ImGui::GetStyle( ).FramePadding.x * 2.0f;
        const float avail = ImGui::GetContentRegionAvail( ).x;
        const float offset = ( avail - button_width ) * 0.5f;
        ImGui::SetCursorPosX( ImGui::GetCursorPosX( ) + offset );
        if ( ImGui::Button( "Refresh" ) )
        {
            m_cached_prcesses = native::g_core->get_all_processes( );
            m_selected_process = k_invalid_index;
        }
        ImGui::EndPopup( );
    }
}

void c_gui::hanlde_attach_process( std::optional< native::process_t > prev, std::optional< native::process_t > curr )
{
    if ( !curr.has_value( ) || m_selected_memory_class >= m_memory_classes.size( ) )
    {
        return;
    }
    memory_class_t& klass = m_memory_classes[ m_selected_memory_class ];
    if ( klass.m_address != 0 )
    {
        return;
    }
    const std::vector< native::module_t > modules = native::g_core->get_modules( );
    if ( modules.empty( ) )
    {
        return;
    }
    klass.m_address = modules.front( ).m_base;
    sync_class_inputs( klass );
}

void c_gui::draw_main_window( const app::window_params_t& params )
{
    handle_attach_popup( );
    draw_memory_window( params );
}

void c_gui::draw_memory_window( const app::window_params_t& params )
{
    ImGui::BeginChild( "##memory_panel", ImGui::GetContentRegionAvail( ), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
    {
        handle_add_class_popup( );
        draw_memory_class_panel( );
        ImGui::SameLine( );
        draw_memory_editor_panel( );
    }
    ImGui::EndChild( );
}

void c_gui::draw_memory_class_panel( )
{
    constexpr float panel_width = 135.0f;
    constexpr float footer_height = 34.0f;
    ImGui::BeginChild( "##memory_class_panel", ImVec2( panel_width, 0.0f ), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
    {
        ImGui::TextUnformatted( "Class" );
        ImGui::Separator( );
        const float list_height = std::max( 0.0f, ImGui::GetContentRegionAvail( ).y - footer_height );
        ImGui::BeginChild( "##memory_class_list", ImVec2( 0.0f, list_height ), false, ImGuiWindowFlags_NoScrollbar );
        {
            for ( std::size_t i = 0; i < m_memory_classes.size( ); ++i )
            {
                const bool selected = m_selected_memory_class == i;
                const std::string id = std::format( "{}##memory_class_{}", m_memory_classes[ i ].m_name, i );
                if ( ImGui::Selectable( id.c_str( ), selected ) )
                {
                    m_selected_memory_class = i;
                    sync_class_inputs( m_memory_classes[ i ] );
                }
            }
        }
        ImGui::EndChild( );
        ImGui::Separator( );
        const float button_size = ImGui::GetFrameHeight( );
        const float buttons_width = button_size * 2.0f + ImGui::GetStyle( ).ItemSpacing.x;
        const float offset = ( ImGui::GetContentRegionAvail( ).x - buttons_width ) * 0.5f;
        if ( offset > 0.0f )
        {
            ImGui::SetCursorPosX( ImGui::GetCursorPosX( ) + offset );
        }
        if ( ImGui::Button( "+", ImVec2( button_size, button_size ) ) )
        {
            std::memset( m_new_class_name.data( ), 0, m_new_class_name.size( ) );
            m_open_add_class_popup = true;
        }
        ImGui::SameLine( );
        const bool can_remove = m_selected_memory_class < m_memory_classes.size( );
        ImGui::BeginDisabled( !can_remove );
        if ( ImGui::Button( "-", ImVec2( button_size, button_size ) ) )
        {
            remove_selected_memory_class( );
        }
        ImGui::EndDisabled( );
    }
    ImGui::EndChild( );
}

void c_gui::draw_memory_editor_panel( )
{
    ImGui::BeginChild( "##memory_editor_panel", ImGui::GetContentRegionAvail( ), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
    {
        if ( m_selected_memory_class >= m_memory_classes.size( ) )
        {
            ImGui::TextUnformatted( "No class selected" );
            ImGui::EndChild( );
            return;
        }
        memory_class_t& klass = m_memory_classes[ m_selected_memory_class ];
        draw_memory_toolbar( klass );
        ImGui::Separator( );
        draw_memory_view( klass );
    }
    ImGui::EndChild( );
}

void c_gui::draw_memory_toolbar( memory_class_t& klass )
{
    ImGui::TextUnformatted( "ADDRESS:" );
    ImGui::SameLine( );
    ImGui::SetNextItemWidth( 170.0f );
    if ( ImGui::InputText( "##memory_address", m_address_input.data( ), m_address_input.size( ), ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue ) )
    {
        native::address_t address{ };
        if ( parse_hex_address( m_address_input.data( ), address ) )
        {
            klass.m_address = address;
        }
    }
    ImGui::SameLine( );
    ImGui::TextUnformatted( "SIZE:" );
    ImGui::SameLine( );
    ImGui::SetNextItemWidth( 75.0f );
    if ( ImGui::InputText( "##memory_size", m_size_input.data( ), m_size_input.size( ), ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue ) )
    {
        std::size_t size{ };
        if ( parse_size_value( m_size_input.data( ), size ) )
        {
            klass.m_size = std::clamp< std::size_t >( size, 0x8, 0x4000 );
            if ( klass.m_selected_offset >= klass.m_size )
            {
                klass.m_selected_offset = 0;
            }
            normalize_nodes( klass );
        }
    }
    ImGui::SameLine( );
    ImGui::TextUnformatted( "TYPES:" );
    const auto button = [ this, &klass ]( const char* label, const e_memory_node_type type )
    {
        ImGui::SameLine( );
        if ( ImGui::Button( label ) )
        {
            apply_node_type( klass, type );
        }
    };
    button( "HEX", e_memory_node_type::k_hex );
    button( "I32", e_memory_node_type::k_int32 );
    button( "U32", e_memory_node_type::k_uint32 );
    button( "F32", e_memory_node_type::k_float );
    button( "PTR", e_memory_node_type::k_pointer );
    button( "STR", e_memory_node_type::k_string );
    button( "PSTR", e_memory_node_type::k_pointer_string );
    ImGui::SameLine( );
    ImGui::TextDisabled( "selected: +0x%zX", klass.m_selected_offset );
}

void c_gui::draw_memory_view( memory_class_t& klass )
{
    constexpr std::size_t max_read_size = 0x4000;
    const std::size_t read_size = std::min( klass.m_size, max_read_size );
    std::vector< std::byte > buffer( read_size );
    native::memory_read_result_t read_result{ };
    if ( klass.m_address != 0 && read_size > 0 )
    {
        read_result = native::g_core->read_memory( klass.m_address, std::span< std::byte >( buffer.data( ), buffer.size( ) ) );
    }
    const bool valid_memory = read_result.m_success && read_result.m_bytes_read > 0;
    if ( !native::g_core->is_process_valid( ) )
    {
        ImGui::TextColored( ImColor{ 255, 90, 90, 255 }.Value, "No process attached" );
    }
    else if ( klass.m_address == 0 )
    {
        ImGui::TextColored( ImColor{ 255, 180, 90, 255 }.Value, "Address is zero" );
    }
    else if ( !valid_memory )
    {
        ImGui::TextColored( ImColor{ 255, 90, 90, 255 }.Value, "Read failed: %d", read_result.m_error_code );
    }
    const ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable;
    if ( ImGui::BeginTable( "##memory_view_table", 5, flags, ImGui::GetContentRegionAvail( ) ) )
    {
        ImGui::TableSetupScrollFreeze( 0, 1 );
        ImGui::TableSetupColumn( "Offset", ImGuiTableColumnFlags_WidthFixed, 70.0f );
        ImGui::TableSetupColumn( "Address", ImGuiTableColumnFlags_WidthFixed, 170.0f );
        ImGui::TableSetupColumn( "Bytes", ImGuiTableColumnFlags_WidthFixed, 310.0f );
        ImGui::TableSetupColumn( "Type", ImGuiTableColumnFlags_WidthFixed, 75.0f );
        ImGui::TableSetupColumn( "Value", ImGuiTableColumnFlags_WidthStretch, 260.0f );
        ImGui::TableHeadersRow( );
        std::size_t offset{ };
        while ( offset < klass.m_size )
        {
            memory_node_t node = make_display_node( klass, offset );
            if ( node.m_size == 0 )
            {
                break;
            }
            draw_memory_row( klass, buffer, read_result.m_bytes_read, node, valid_memory );
            offset += node.m_size;
        }
        ImGui::EndTable( );
    }
}

void c_gui::draw_memory_row( memory_class_t& klass, const std::vector< std::byte >& buffer, const std::size_t bytes_read, const memory_node_t& node, const bool valid_memory )
{
    const bool selected = klass.m_selected_offset == node.m_offset;
    const bool readable = valid_memory && node.m_offset < bytes_read;
    const std::size_t available = readable ? std::min( node.m_size, bytes_read - node.m_offset ) : 0;
    ImGui::TableNextRow( );
    ImGui::TableSetColumnIndex( 0 );
    ImGui::PushID( static_cast< int >( node.m_offset ) );
    const std::string offset_text = std::format( "{:04X}", node.m_offset );
    if ( ImGui::Selectable( offset_text.c_str( ), selected, ImGuiSelectableFlags_SpanAllColumns ) )
    {
        klass.m_selected_offset = node.m_offset;
    }
    ImGui::TableSetColumnIndex( 1 );
    ImGui::TextColored( ImColor{ 60, 255, 80, 255 }.Value, "%s", format_address( klass.m_address + node.m_offset ).c_str( ) );
    ImGui::TableSetColumnIndex( 2 );
    std::string bytes_string{ };
    constexpr std::size_t max_visible_bytes = 16;
    const std::size_t visible_bytes = std::min( node.m_size, max_visible_bytes );
    for ( std::size_t i = 0; i < visible_bytes; ++i )
    {
        if ( i != 0 )
        {
            bytes_string += ' ';
        }
        if ( i < available )
        {
            bytes_string += std::format( "{:02X}", static_cast< unsigned int >( std::to_integer< unsigned char >( buffer[ node.m_offset + i ] ) ) );
        }
        else
        {
            bytes_string += "??";
        }
    }
    if ( node.m_size > max_visible_bytes )
    {
        bytes_string += " ...";
    }
    ImGui::TextUnformatted( bytes_string.c_str( ) );
    ImGui::TableSetColumnIndex( 3 );
    ImGui::TextColored( ImColor{ 80, 170, 255, 255 }.Value, "%s", node_type_name( node.m_type ).c_str( ) );
    ImGui::TableSetColumnIndex( 4 );
    if ( available < node.m_size && node.m_type != e_memory_node_type::k_string )
    {
        ImGui::TextDisabled( "unreadable" );
        ImGui::PopID( );
        return;
    }
    if ( available == 0 )
    {
        ImGui::TextDisabled( "unreadable" );
        ImGui::PopID( );
        return;
    }
    std::string value_text{ };
    switch ( node.m_type )
    {
    case e_memory_node_type::k_hex:
    {
        std::uint64_t value{ };
        const std::size_t copy_size = std::min( sizeof( value ), node.m_size );
        std::memcpy( &value, buffer.data( ) + node.m_offset, copy_size );
        value_text = std::format( "0x{:0{}X}", static_cast< unsigned long long >( value ), copy_size * 2 );
        break;
    }
    case e_memory_node_type::k_int32:
    {
        std::int32_t value{ };
        if ( read_value( buffer, node.m_offset, value ) )
        {
            value_text = std::format( "{}", value );
        }
        break;
    }
    case e_memory_node_type::k_uint32:
    {
        std::uint32_t value{ };
        if ( read_value( buffer, node.m_offset, value ) )
        {
            value_text = std::format( "{}", value );
        }
        break;
    }
    case e_memory_node_type::k_float:
    {
        float value{ };
        if ( read_value( buffer, node.m_offset, value ) )
        {
            value_text = std::format( "{:.6f}", value );
        }
        break;
    }
    case e_memory_node_type::k_pointer:
    {
        native::address_t value{ };
        if ( read_value( buffer, node.m_offset, value ) )
        {
            value_text = format_address( value );
        }
        break;
    }
    case e_memory_node_type::k_string:
    {
        value_text = std::format( "\"{}\"", format_ascii_string( buffer, node.m_offset, available ) );
        break;
    }
    case e_memory_node_type::k_pointer_string:
    {
        native::address_t string_address{ };
        if ( !read_value( buffer, node.m_offset, string_address ) || string_address == 0 )
        {
            ImGui::TextDisabled( "null" );
            ImGui::PopID( );
            return;
        }
        value_text = std::format( "{} -> \"{}\"", format_address( string_address ), read_remote_ascii_string( string_address, 0x100 ) );
        break;
    }
    default:
    {
        ImGui::TextDisabled( "unknown" );
        ImGui::PopID( );
        return;
    }
    }
    draw_copyable_value( value_text, ImColor{ 255, 150, 70, 255 }.Value );
    ImGui::PopID( );
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
            draw_main_window( params );
            break;
        }
        case k_debug:
        {
            draw_debug_window( params );
            break;
        }
        default:
        {
            break;
        }
        }
    }
    ImGui::End( );
}

void c_gui::handle_add_class_popup( )
{
    if ( m_open_add_class_popup )
    {
        ImGui::OpenPopup( "Add class" );
        m_open_add_class_popup = false;
    }
    ImGui::SetNextWindowPos( ImGui::GetMainViewport( )->GetCenter( ), ImGuiCond_Appearing, ImVec2( 0.5f, 0.5f ) );
    if ( ImGui::BeginPopup( "Add class", ImGuiWindowFlags_NoMove ) )
    {
        ImGui::TextUnformatted( "Class name" );
        ImGui::SetNextItemWidth( 220.0f );
        const bool enter_pressed = ImGui::InputText( "##new_class_name", m_new_class_name.data( ), m_new_class_name.size( ), ImGuiInputTextFlags_EnterReturnsTrue );
        const bool add_pressed = ImGui::Button( "Add", ImVec2( 90.0f, 0.0f ) );
        ImGui::SameLine( );
        if ( ImGui::Button( "Cancel", ImVec2( 90.0f, 0.0f ) ) )
        {
            ImGui::CloseCurrentPopup( );
        }
        if ( enter_pressed || add_pressed )
        {
            if ( add_memory_class( m_new_class_name.data( ) ) )
            {
                std::memset( m_new_class_name.data( ), 0, m_new_class_name.size( ) );
                ImGui::CloseCurrentPopup( );
            }
        }
        ImGui::EndPopup( );
    }
}

bool c_gui::add_memory_class( std::string_view name )
{
    while ( !name.empty( ) && std::isspace( static_cast< unsigned char >( name.front( ) ) ) )
    {
        name.remove_prefix( 1 );
    }
    while ( !name.empty( ) && std::isspace( static_cast< unsigned char >( name.back( ) ) ) )
    {
        name.remove_suffix( 1 );
    }
    if ( name.empty( ) || name.size( ) >= m_new_class_name.size( ) )
    {
        return false;
    }
    const auto exists = std::ranges::any_of( m_memory_classes, [ name ]( const memory_class_t& item )
                                            {
                                                return item.m_name == name;
                                            } );
    if ( exists )
    {
        return false;
    }
    memory_class_t klass{ };
    klass.m_name = std::string{ name };
    klass.m_size = 0x80;
    klass.m_selected_offset = 0;
    m_memory_classes.emplace_back( std::move( klass ) );
    m_selected_memory_class = m_memory_classes.size( ) - 1;
    sync_class_inputs( m_memory_classes.back( ) );
    return true;
}

void c_gui::remove_selected_memory_class( )
{
    if ( m_selected_memory_class >= m_memory_classes.size( ) )
    {
        return;
    }
    m_memory_classes.erase( m_memory_classes.begin( ) + static_cast< std::ptrdiff_t >( m_selected_memory_class ) );
    if ( m_memory_classes.empty( ) )
    {
        m_selected_memory_class = k_invalid_index;
        std::memset( m_address_input.data( ), 0, m_address_input.size( ) );
        std::memset( m_size_input.data( ), 0, m_size_input.size( ) );
        return;
    }
    if ( m_selected_memory_class >= m_memory_classes.size( ) )
    {
        m_selected_memory_class = m_memory_classes.size( ) - 1;
    }
    sync_class_inputs( m_memory_classes[ m_selected_memory_class ] );
}

void c_gui::apply_node_type( memory_class_t& klass, const e_memory_node_type type )
{
    if ( klass.m_selected_offset >= klass.m_size )
    {
        return;
    }
    if ( type == e_memory_node_type::k_hex )
    {
        remove_node_at_offset( klass, klass.m_selected_offset );
        normalize_nodes( klass );
        return;
    }
    std::size_t size = node_type_size( type );
    if ( size == 0 )
    {
        return;
    }
    if ( klass.m_selected_offset + size > klass.m_size )
    {
        size = klass.m_size - klass.m_selected_offset;
    }
    if ( size == 0 )
    {
        return;
    }
    const std::size_t begin = klass.m_selected_offset;
    const std::size_t end = begin + size;
    std::erase_if( klass.m_nodes, [ begin, end ]( const memory_node_t& node )
                  {
                      const std::size_t node_begin = node.m_offset;
                      const std::size_t node_end = node.m_offset + node.m_size;
                      return node_begin < end && begin < node_end;
                  } );
    klass.m_nodes.emplace_back( memory_node_t{ .m_offset = begin, .m_size = size, .m_type = type } );
    normalize_nodes( klass );
}

void c_gui::remove_node_at_offset( memory_class_t& klass, const std::size_t offset )
{
    std::erase_if( klass.m_nodes, [ offset ]( const memory_node_t& node )
                  {
                      return node.m_offset == offset || ( offset > node.m_offset && offset < node.m_offset + node.m_size );
                  } );
}

void c_gui::normalize_nodes( memory_class_t& klass )
{
    std::erase_if( klass.m_nodes, [ &klass ]( const memory_node_t& node )
                  {
                      return node.m_size == 0 || node.m_offset >= klass.m_size || node.m_offset + node.m_size > klass.m_size || node.m_type == e_memory_node_type::k_hex;
                  } );
    std::ranges::sort( klass.m_nodes, { }, &memory_node_t::m_offset );
}

void c_gui::sync_class_inputs( const memory_class_t& klass )
{
    const std::string address = std::format( "{:X}", klass.m_address );
    const std::string size = std::format( "{:X}", klass.m_size );
    std::memset( m_address_input.data( ), 0, m_address_input.size( ) );
    std::memset( m_size_input.data( ), 0, m_size_input.size( ) );
    std::strncpy( m_address_input.data( ), address.c_str( ), m_address_input.size( ) - 1 );
    std::strncpy( m_size_input.data( ), size.c_str( ), m_size_input.size( ) - 1 );
}

int c_gui::get_window_flags( ) const
{
    return ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar;
}

bool c_gui::parse_hex_address( std::string_view text, native::address_t& out ) const
{
    while ( !text.empty( ) && std::isspace( static_cast< unsigned char >( text.front( ) ) ) )
    {
        text.remove_prefix( 1 );
    }
    while ( !text.empty( ) && std::isspace( static_cast< unsigned char >( text.back( ) ) ) )
    {
        text.remove_suffix( 1 );
    }
    if ( text.starts_with( "0x" ) || text.starts_with( "0X" ) )
    {
        text.remove_prefix( 2 );
    }
    if ( text.empty( ) )
    {
        return false;
    }
    native::address_t value{ };
    const auto [ ptr, ec ] = std::from_chars( text.data( ), text.data( ) + text.size( ), value, 16 );
    if ( ec != std::errc{ } || ptr != text.data( ) + text.size( ) )
    {
        return false;
    }
    out = value;
    return true;
}

bool c_gui::parse_size_value( std::string_view text, std::size_t& out ) const
{
    while ( !text.empty( ) && std::isspace( static_cast< unsigned char >( text.front( ) ) ) )
    {
        text.remove_prefix( 1 );
    }
    while ( !text.empty( ) && std::isspace( static_cast< unsigned char >( text.back( ) ) ) )
    {
        text.remove_suffix( 1 );
    }
    if ( text.starts_with( "0x" ) || text.starts_with( "0X" ) )
    {
        text.remove_prefix( 2 );
    }
    if ( text.empty( ) )
    {
        return false;
    }
    std::size_t value{ };
    const auto [ ptr, ec ] = std::from_chars( text.data( ), text.data( ) + text.size( ), value, 16 );
    if ( ec != std::errc{ } || ptr != text.data( ) + text.size( ) )
    {
        return false;
    }
    out = value;
    return true;
}

std::string c_gui::format_address( const native::address_t address ) const
{
    if constexpr ( sizeof( native::address_t ) == 8 )
    {
        return std::format( "{:016X}", address );
    }
    else
    {
        return std::format( "{:08X}", address );
    }
}

std::string c_gui::node_type_name( const e_memory_node_type type ) const
{
    switch ( type )
    {
    case e_memory_node_type::k_hex:
    {
        return "hex";
    }
    case e_memory_node_type::k_int32:
    {
        return "int32";
    }
    case e_memory_node_type::k_uint32:
    {
        return "uint32";
    }
    case e_memory_node_type::k_float:
    {
        return "float";
    }
    case e_memory_node_type::k_pointer:
    {
        return "ptr";
    }
    case e_memory_node_type::k_string:
    {
        return "string";
    }
    case e_memory_node_type::k_pointer_string:
    {
        return "pstr";
    }
    default:
    {
        return "unknown";
    }
    }
}

std::size_t c_gui::node_type_size( const e_memory_node_type type ) const
{
    switch ( type )
    {
    case e_memory_node_type::k_hex:
    {
        return 8;
    }
    case e_memory_node_type::k_int32:
    case e_memory_node_type::k_uint32:
    case e_memory_node_type::k_float:
    {
        return 4;
    }
    case e_memory_node_type::k_pointer:
    case e_memory_node_type::k_pointer_string:
    {
        return sizeof( native::address_t );
    }
    case e_memory_node_type::k_string:
    {
        return 0x20;
    }
    default:
    {
        return 0;
    }
    }
}

std::optional< memory_node_t > c_gui::find_node_at_offset( const memory_class_t& klass, const std::size_t offset ) const
{
    const auto it = std::ranges::find_if( klass.m_nodes, [ offset ]( const memory_node_t& node )
                                         {
                                             return node.m_offset == offset;
                                         } );
    if ( it == klass.m_nodes.end( ) )
    {
        return std::nullopt;
    }
    return *it;
}

std::optional< memory_node_t > c_gui::find_next_node_after( const memory_class_t& klass, const std::size_t offset ) const
{
    const auto it = std::ranges::find_if( klass.m_nodes, [ offset ]( const memory_node_t& node )
                                         {
                                             return node.m_offset > offset;
                                         } );
    if ( it == klass.m_nodes.end( ) )
    {
        return std::nullopt;
    }
    return *it;
}

memory_node_t c_gui::make_display_node( const memory_class_t& klass, const std::size_t offset ) const
{
    if ( const auto node = find_node_at_offset( klass, offset ); node.has_value( ) )
    {
        return *node;
    }
    constexpr std::size_t default_hex_size = 8;
    std::size_t size = std::min( default_hex_size, klass.m_size - offset );
    if ( const auto next = find_next_node_after( klass, offset ); next.has_value( ) )
    {
        size = std::min( size, next->m_offset - offset );
    }
    return memory_node_t{ .m_offset = offset, .m_size = size, .m_type = e_memory_node_type::k_hex };
}

std::string c_gui::format_ascii_string( const std::vector< std::byte >& buffer, const std::size_t offset, const std::size_t size ) const
{
    if ( offset >= buffer.size( ) || size == 0 )
    {
        return std::string{ };
    }
    constexpr std::size_t max_visible_chars = 256;
    const std::size_t available = std::min( { size, buffer.size( ) - offset, max_visible_chars } );
    std::string result{ };
    result.reserve( available );
    for ( std::size_t i = 0; i < available; ++i )
    {
        const unsigned char ch = std::to_integer< unsigned char >( buffer[ offset + i ] );
        if ( ch == 0 )
        {
            break;
        }
        if ( ch == '\\' )
        {
            result += "\\\\";
            continue;
        }
        if ( ch == '"' )
        {
            result += "\\\"";
            continue;
        }
        if ( std::isprint( ch ) != 0 )
        {
            result.push_back( static_cast< char >( ch ) );
            continue;
        }
        result.push_back( '.' );
    }
    if ( result.size( ) == max_visible_chars )
    {
        result += "...";
    }
    return result;
}

std::string c_gui::read_remote_ascii_string( const native::address_t address, const std::size_t max_size ) const
{
    if ( address == 0 || max_size == 0 || !native::g_core->is_process_valid( ) )
    {
        return std::string{ };
    }
    constexpr std::size_t hard_limit = 0x1000;
    const std::size_t read_size = std::min( max_size, hard_limit );
    std::vector< std::byte > buffer( read_size );
    const native::memory_read_result_t result = native::g_core->read_memory( address, std::span< std::byte >( buffer.data( ), buffer.size( ) ) );
    if ( !result.m_success || result.m_bytes_read == 0 )
    {
        return std::string{ };
    }
    return format_ascii_string( buffer, 0, result.m_bytes_read );
}

} // namespace zircon::gui
