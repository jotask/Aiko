#pragma once

#include <vector>
#include <cstdint>

#include "aiko_types.h"
#include "types/render_types.h"   // for ViewId, ComputeAccess
#include "models/texture.h"

namespace aiko
{
    class ComputeBuffer;
    class ComputeShader;

    struct ComputeImageBinding
    {
        uint8_t stage = 0;
        const Texture* texture = nullptr;
        ComputeAccess access = ComputeAccess::Write;
    };

    struct ComputeDispatch
    {
        u32 groupsX = 1;
        u32 groupsY = 1;
        u32 groupsZ = 1;
    };

    enum class ComputeBufferFormat
    {
        Vec4f,   // float4
        // TODO:
        Vec2f,
        Vec3f,
        Mat4f,   // 4x vec4
    };

    struct ComputeBufferBinding
    {
        uint8_t stage = 0;
        const ComputeBuffer* buffer = nullptr;
        ComputeAccess access = ComputeAccess::ReadWrite;
    };

    struct ComputePass
    {
        ViewId viewId = 0;
        const ComputeShader* shader = nullptr;

        std::vector<ComputeImageBinding> images;
        ComputeDispatch dispatch;

        // Optional debug name (useful later)
        const char* name = nullptr;

        std::vector<ComputeBufferBinding> buffers;
    };

}