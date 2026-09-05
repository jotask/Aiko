#pragma once

#include <aiko_types.h>

namespace aiko
{
    struct RendererConfig
    {
        bool vsync = true;
    };

    struct RenderSurfaceDesc
    {
        void* nativeWindowHandle = nullptr;
        u32 width = 0;
        u32 height = 0;
    };
}
