#pragma once

namespace zircon::gui
{

struct window_impl_t;

class c_draw_wrapper
{
public:
    void init( window_impl_t* window );
    void begin_frame( window_impl_t* window );
    void end_frame( window_impl_t* window );
    void cleanup( );

public:
    float get_delta_time( ) const;

};

} // namespace zircon::gui
