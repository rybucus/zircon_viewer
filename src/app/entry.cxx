#include <set>
#include <string>

#include <app/c_application.hxx>

int main( int argc, char* argv[ ] )
{
    const auto make_args = [ ] ( int argc, char* argv[ ] ) -> std::set< std::string > {
        return std::views::counted( argv, argc ) | std::views::transform( [ ] ( auto* var ) { return std::string{ var }; } )
            | std::ranges::to< std::set< std::string > >( );
    };

    zircon::app::g_application = std::make_unique< zircon::app::c_application >( );

    zircon::app::g_application->on_load( make_args( argc, argv ) );

    zircon::app::g_application->run( );

    return 0;
}
