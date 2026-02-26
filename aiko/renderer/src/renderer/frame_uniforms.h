#pragma

#include <math/math_vector.h>

#include "types/color.h"

namespace aiko::renderer
{

    struct DeviceInitDesc
    {
        void* nativeWindowHandle = nullptr;
        u32 width = 0;
        u32 height = 0;
        bool vsync = false;
    };

    struct PassDescription
    {
        u32 width;
        u32 height;
        bool clearColor = true;
        bool clearDepth = true;
        Color clear = BLACK;
    };

    struct DirectionalLight
    {
        vec3 direction = vec3(-1.0f, -1.f, -1.0f);
        Color color = WHITE;
        float intensity = 1.0f;
    };

    struct AmbientLight
    {
        Color color = WHITE;
        float intensity = 1.0f;
    };

    struct FrameUniforms
    {
        mat4 view = mat4(1.0f);
        mat4 projection = mat4(1.0f);
        vec3 cameraPosition = vec3(0.0f);

        AmbientLight ambient;
        DirectionalLight sun;
    };

}