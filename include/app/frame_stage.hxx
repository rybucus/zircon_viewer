#pragma once

#include <cstdint>
#include <limits>
#include <optional>

namespace zircon::app
{

enum e_frame_stage : std::uint8_t
{
    k_stage_invalid = std::numeric_limits< std::uint8_t >::max( ),
    k_stage_begin = 0,
    k_stage_end,
    k_input_handled,
    k_init,
    k_stage_count
};

struct window_params_t
{
    float m_window_width{ };
    float m_window_heigth{ };
    float m_window_scale{ };
    float m_delta_time{ };
};

struct frame_state_t
{
public:
    bool is_valid( ) const { return m_stage != k_stage_invalid; }

public:
    e_frame_stage m_stage{ };
    std::optional< window_params_t > m_params{ };
};

} // namespace zircon::app
