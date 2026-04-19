#pragma once

#include <memory>

#include <utils/json.hxx>
#include <app/frame_stage.hxx>

namespace zircon::app
{

class i_application;

class i_module
{
public:
    virtual ~i_module( ) = default;
    virtual void on_attach( i_application* application ) { }
    virtual void on_detach( i_application* application ) { }
    virtual void on_frame_stage_notify( const frame_state_t& frame ) { }
    virtual void on_config_load( const utils::json_t& config ) { }

public:
    template< typename T > requires( std::is_base_of_v< i_module, T > )
    std::shared_ptr< T > as( ) const noexcept
    {
        return dynamic_cast< T >( this );
    }

};

} // namespace zircon::app
