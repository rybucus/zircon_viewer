cmake_minimum_required(VERSION 3.21)

project(zircon VERSION 1.0 LANGUAGES CXX)

option(ZIRCON_USE_LIBCXX "Build with libc++ when using Clang" OFF)

if (ZIRCON_USE_LIBCXX)
    if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(FATAL_ERROR "ZIRCON_USE_LIBCXX requires Clang")
    endif()

    add_compile_options(-stdlib=libc++)
    add_link_options(-stdlib=libc++)
    add_link_options(-fuse-ld=lld)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(fmt REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

set(IMGUI_DIR ${PROJECT_SOURCE_DIR}/external/imgui)

add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui
PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)


target_link_libraries(imgui
PUBLIC
    glfw
    OpenGL::GL
)

target_compile_features(imgui PUBLIC cxx_std_23)

add_subdirectory(src)
