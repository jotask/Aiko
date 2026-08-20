#pragma once

#include <string>
#include <unordered_map>

#if defined(AIKO_BGFX)
    #include <bgfx/bgfx.h>

    // IMGUI
    #include <bgfx/embedded_shader.h>
    #include <bx/math.h>
    #include <bx/timer.h>
#elif defined(AIKO_NATIVE)
    #error Not Implemented!
#elif defined(AIKO_VULKAN)
    #include <volk.h>
#else
    #error Not supported!
#endif

#include <GLFW/glfw3.h>

#include <magic_enum/magic_enum.hpp>