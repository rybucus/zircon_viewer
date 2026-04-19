add_library(zircon_gui STATIC
    ../../include/gui/c_window.hxx
    ../../include/gui/c_draw_wrapper.hxx
    ../../include/gui/window_impl_t.hxx
    c_window.cxx
    c_draw_wrapper.cxx
    ../../include/gui/c_gui.hxx
    c_gui.cxx
    ../../include/gui/verdana.hxx
)

target_include_directories(zircon_gui
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(zircon_gui
	PUBLIC
        	fmt::fmt
        	nlohmann_json::nlohmann_json
        	imgui
	PRIVATE
		zircon_utils
)
