#pragma once

#include <app/i_module.hxx>

namespace zircon::gui
{

enum e_window_type
{
    k_main,
    k_debug
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

private:
    int get_window_flags( ) const;

private:
    e_window_type m_selected_window{ k_main };

};

inline c_gui g_gui{ };

} // namespace zircon::gui
