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

    enum class ComputeBufferUsage : uint32_t
    {
        None        = 0,
        Storage     = 1u << 0,
        TransferSrc = 1u << 1,
        TransferDst = 1u << 2,
        Vertex      = 1u << 3,
        Index       = 1u << 4,
        Indirect    = 1u << 5,
    };

    inline ComputeBufferUsage operator|(ComputeBufferUsage lhs, ComputeBufferUsage rhs)
    {
        return static_cast<ComputeBufferUsage>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
    }

    inline ComputeBufferUsage operator&(ComputeBufferUsage lhs, ComputeBufferUsage rhs)
    {
        return static_cast<ComputeBufferUsage>(static_cast<uint32_t>(lhs) &static_cast<uint32_t>(rhs));
    }

    inline bool hasFlag(ComputeBufferUsage value, ComputeBufferUsage flag)
    {
        return (static_cast<uint32_t>(value) & static_cast<uint32_t>(flag)) != 0;
    }

    struct ComputeBufferDesc
    {
        ComputeBufferFormat format = ComputeBufferFormat::Vec4f;
        uint32_t count = 0;
        ComputeBufferUsage usage = ComputeBufferUsage::Storage | ComputeBufferUsage::TransferSrc | ComputeBufferUsage::TransferDst;
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
        uint8_t slot = 0;
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