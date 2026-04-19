#pragma once

#include <string>
#include <string_view>

#include <utils/json.hxx>

namespace zircon::utils
{

std::string get_current_dir( ) noexcept;

bool is_directory_exists( const std::string_view path ) noexcept;

bool is_file_exists( const std::string_view path ) noexcept;

bool create_directories_if_not_exists( const std::string_view path ) noexcept;

bool create_file_if_not_exists( const std::string_view path, const std::string& data = { } ) noexcept;

json_t load_json_file( const std::string_view path, const std::string& data = { } );

} // namespace zircon::utils
