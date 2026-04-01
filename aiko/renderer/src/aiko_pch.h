#pragma once

#include <string>
#include <unordered_map>

#include <GLFW/glfw3.h>

#if defined(AIKO_BGFX)
    #include <bgfx/bgfx.h>

    // IMGUI
    #include <bgfx/embedded_shader.h>
    #include <bx/math.h>
    #include <bx/timer.h>
#elif defined(AIKO_NATIVE)
#else
    #error Not supported!
#endif

#include <magic_enum/magic_enum.hpp>