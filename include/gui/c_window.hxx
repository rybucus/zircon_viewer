#pragma once

#include <memory>

#include <app/i_module.hxx>

#include <utils/json.hxx>

namespace zircon::gui
{

struct window_impl_t;

struct window_preset_t : public app::i_module
{
public:
    static constexpr std::string_view k_key = "window";
    static constexpr float k_default_width = 1280.f;
    static constexpr float k_default_heigth = 800.f;
    static constexpr std::string_view k_default_name = "zircon";
    static constexpr bool k_vsync_default = false;
    static constexpr float k_scale_default = -1.f;

public:
    void on_config_load( const utils::json_t& config ) override;

public:
    float m_width{ k_default_width };
    float m_heigth{ k_default_heigth };
    std::string m_name{ k_default_name };
    bool m_should_use_vsync{ k_vsync_default };
    float m_override_scale{ k_scale_default };

};

inline window_preset_t g_window_settings{ };

class c_window
{
public:
    c_window( );

    ~c_window( );

public:
    void catch_events( );

public:
    bool is_should_close( ) const;
    bool is_unfocused( ) const;

public:
    window_impl_t* get_impl( ) const;
    float get_window_width( ) const;
    float get_window_heigth( ) const;
    float get_window_scale( ) const;

private:
    std::unique_ptr< window_impl_t > m_impl{ };

};

} // namespace zircon::gui
