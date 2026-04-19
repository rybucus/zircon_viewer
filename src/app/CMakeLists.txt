add_executable(zircon
    ../../include/app/i_application.hxx
    ../../include/app/i_module.hxx
    ../../include/app/frame_stage.hxx


    ../../include/app/c_application.hxx
    ../../include/app/application_config.hxx
    c_application.cxx
    entry.cxx
)

target_include_directories(zircon
PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(zircon
PUBLIC
    fmt::fmt
    nlohmann_json::nlohmann_json
PRIVATE
    zircon_gui
    zircon_core
    zircon_utils
)
