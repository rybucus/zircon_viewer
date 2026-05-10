#pragma once

#include <app/i_module.hxx>
#include <core/i_core.hxx>

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace zircon::gui
{

enum e_window_type
{
    k_main,
    k_debug
};

enum class e_memory_node_type : std::uint8_t
{
    k_hex,
    k_int32,
    k_uint32,
    k_float,
    k_pointer,
    k_string,
    k_pointer_string
};

struct memory_node_t
{
public:
    std::size_t m_offset{ };
    std::size_t m_size{ };
    e_memory_node_type m_type{ e_memory_node_type::k_hex };
};

struct memory_class_t
{
public:
    std::string m_name{ };
    native::address_t m_address{ };
    std::size_t m_size{ 0x80 };
    std::size_t m_selected_offset{ };
    std::vector< memory_node_t > m_nodes{ };
};

class c_gui : public app::i_module
{
public:
    static constexpr std::string_view k_key = "gui";

public:
    void on_frame_stage_notify( const app::frame_state_t& frame ) override;
    void on_config_load( const utils::json_t& config ) override;

private:
    void on_draw_begin( const app::window_params_t& params );
    void prepare_window( const app::window_params_t& params );
    void prepare_style( );
    void prepare_fonts( );

private:
    void draw_tooltip_bar( const app::window_params_t& params );
    void draw_debug_window( const app::window_params_t& params );
    void draw_main_window( const app::window_params_t& params );
    void draw_memory_window( const app::window_params_t& params );
    void draw_memory_class_panel( );
    void draw_memory_editor_panel( );
    void draw_memory_toolbar( memory_class_t& klass );
    void draw_memory_view( memory_class_t& klass );
    void draw_memory_row( memory_class_t& klass, const std::vector< std::byte >& buffer, std::size_t bytes_read, const memory_node_t& node, bool valid_memory );
    void handle_attach_popup( );
    void handle_add_class_popup( );
    void hanlde_attach_process( std::optional< native::process_t > prev, std::optional< native::process_t > curr );

private:
    bool add_memory_class( std::string_view name );
    void remove_selected_memory_class( );
    void apply_node_type( memory_class_t& klass, e_memory_node_type type );
    void remove_node_at_offset( memory_class_t& klass, std::size_t offset );
    void normalize_nodes( memory_class_t& klass );
    void sync_class_inputs( const memory_class_t& klass );

private:
    [[nodiscard]] int get_window_flags( ) const;
    [[nodiscard]] bool parse_hex_address( std::string_view text, native::address_t& out ) const;
    [[nodiscard]] bool parse_size_value( std::string_view text, std::size_t& out ) const;
    [[nodiscard]] std::string format_address( native::address_t address ) const;
    [[nodiscard]] std::string node_type_name( e_memory_node_type type ) const;
    [[nodiscard]] std::size_t node_type_size( e_memory_node_type type ) const;
    [[nodiscard]] std::optional< memory_node_t > find_node_at_offset( const memory_class_t& klass, std::size_t offset ) const;
    [[nodiscard]] std::optional< memory_node_t > find_next_node_after( const memory_class_t& klass, std::size_t offset ) const;
    [[nodiscard]] memory_node_t make_display_node( const memory_class_t& klass, std::size_t offset ) const;
    [[nodiscard]] std::string format_ascii_string( const std::vector< std::byte >& buffer, std::size_t offset, std::size_t size ) const;
    [[nodiscard]] std::string read_remote_ascii_string( native::address_t address, std::size_t max_size ) const;

private:
    static constexpr std::size_t k_invalid_index = static_cast< std::size_t >( -1 );

private:
    e_window_type m_selected_window{ k_main };
    bool m_open_attach_popup{ };
    bool m_open_add_class_popup{ };
    std::optional< native::process_t > m_attached_process{ };
    std::vector< native::process_t > m_cached_prcesses{ };
    std::size_t m_selected_process{ k_invalid_index };
    std::array< char, 64 > m_new_class_name{ };
    std::array< char, 32 > m_address_input{ };
    std::array< char, 16 > m_size_input{ };
    std::vector< memory_class_t > m_memory_classes{ };
    std::size_t m_selected_memory_class{ k_invalid_index };
};

inline c_gui g_gui{ };

} // namespace zircon::gui
