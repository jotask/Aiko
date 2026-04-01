#pragma once

#include "aiko_types.h"
#include "render_types.h"
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

    using ReadbackId = uint64_t;
    static constexpr ReadbackId InvalidReadbackId = 0;

    struct ComputeReadbackRequest
    {
        ReadbackId id = 0;                 // filled by caller or device
        const ComputeBuffer* buffer = nullptr;
        uint32_t byteSize = 0;
    };

    struct ComputeReadbackResult
    {
        ReadbackId id = 0;                 // filled by caller or device
        bool ready = false;
        vector<uint8_t> data; // raw bytes
    };

    struct ComputeBufferBinding
    {
        uint8_t stage = 0;
        const ComputeBuffer* buffer = nullptr;
        ComputeAccess access = ComputeAccess::ReadWrite;
    };

    struct ComputeVec4Uniform
    {
        const char* name = nullptr;
        vec4 value = vec4(0.0f);
    };

    struct ComputePass
    {
        ComputeShader* shader = nullptr;
        vector<ComputeImageBinding> images;
        ComputeDispatch dispatch;
        vector<ComputeBufferBinding> buffers;
        vector<ComputeVec4Uniform> vec4Uniforms;
    };

}