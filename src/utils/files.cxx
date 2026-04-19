#include <filesystem>
#include <fstream>

#include <utils/files.hxx>

namespace zircon::utils
{

std::string get_current_dir( ) noexcept
{
    return std::filesystem::current_path( ).string( );
}

bool is_directory_exists( const std::string_view path ) noexcept
{
    return !path.empty( ) && std::filesystem::exists( path ) && std::filesystem::is_directory( path );
}

bool is_file_exists( const std::string_view path ) noexcept
{
    return !path.empty( ) && std::filesystem::exists( path ) && std::filesystem::is_regular_file( path );
}

bool create_directories_if_not_exists( const std::string_view path ) noexcept
{
    if ( path.empty( ) || is_directory_exists( path ) )
    {
        return false;
    }

    std::error_code error{ };
    return std::filesystem::create_directories( path, error ) && !error;
}

bool create_file_if_not_exists( const std::string_view path, const std::string& data ) noexcept
{
    if ( is_file_exists( path ) )
    {
        return false;
    }

    create_directories_if_not_exists( std::filesystem::path{ path }.parent_path( ).string( ) );
    std::ofstream file( path, std::ios::out | std::ios::trunc );
    if ( !file.is_open( ) )
    {
        return false;
    }

    if ( !data.empty( ) )
    {
        file << data;
    }

    file.flush( );

    return file.good( );
}

json_t load_json_file( const std::string_view path, const std::string& data )
{
    create_file_if_not_exists( path, data );
    if ( !is_file_exists( path ) )
    {
        return json_t{ };
    }

    std::ifstream file( path );
    if ( !file.is_open( ) )
    {
        return json_t{ };
    }

    json_t out_data{ };
    try
    {
        file >> out_data;
    }
    catch ( ... )
    {
        return json_t{ };
    }

    return out_data;
}

} // namespace zircon::utils
