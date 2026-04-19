add_library(zircon_utils STATIC
    ../../include/utils/logging.hxx
    ../../include/utils/assert.hxx
    ../../include/utils/json.hxx
    ../../include/utils/files.hxx
    files.cxx
)

target_include_directories(zircon_utils
PRIVATE 
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(zircon_utils
PUBLIC
    fmt::fmt
    nlohmann_json::nlohmann_json
)

