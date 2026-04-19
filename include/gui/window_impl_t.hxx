#pragma once

#include <GLFW/glfw3.h>

/*
*
*  Do not include this header in others headers.
*  Include it only in source files.
*
*/

namespace zircon::gui
{

struct window_impl_t
{
    GLFWwindow* m_window{ };
    float m_scale{ };
};

} // namespace zircon::gui
