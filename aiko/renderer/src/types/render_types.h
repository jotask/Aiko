#pragma once

#include <aiko_types.h>

namespace aiko
{
    using ViewId = uint16_t;
    using RenderResourceId = uint64_t;
    using MaterialId = uint64_t;

    constexpr RenderResourceId InvalidRenderResourceId = 0;

    constexpr ViewId COMPUTE_VIEW = 0;
    constexpr ViewId COMPUTE_DRAW = 1;
    constexpr ViewId SCENE_VIEW = 2;
    constexpr ViewId SCREEN_VIEW = 3;
    constexpr ViewId IMGUI_VIEW = 4;

    constexpr ViewId READBACK_VIEW = 250;
    constexpr ViewId READBACK_BLIT_VIEW = 251;

    constexpr uint16_t MAX_LIGHTS = 8;

    struct DeviceInitDesc
    {
        void* nativeWindowHandle = nullptr;

        u32 width = 0;
        u32 height = 0;

        bool vsync = false;
    };
}