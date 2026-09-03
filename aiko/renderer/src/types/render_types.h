#pragma once

#include <aiko_types.h>
#include <math/math_vector.h>
#include "types/color.h"

namespace aiko
{

    class ComputeBuffer;
    class Mesh;
    class Material;

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

    // Shader uniform data type
    enum class ShaderUniformDataType
    {
        SHADER_UNIFORM_FLOAT,
        SHADER_UNIFORM_VEC2,
        SHADER_UNIFORM_VEC3,
        SHADER_UNIFORM_VEC4,
        SHADER_UNIFORM_INT,
        SHADER_UNIFORM_IVEC2,
        SHADER_UNIFORM_IVEC3,
        SHADER_UNIFORM_IVEC4,
        SHADER_UNIFORM_MAT4,
        SHADER_UNIFORM_SAMPLER2D
    } ;

    enum class BlendMode
    {
        BLEND_ALPHA,                    // Blend textures considering alpha (default)
        BLEND_ADDITIVE,                 // Blend textures adding colors
        BLEND_MULTIPLIED,               // Blend textures multiplying colors
        BLEND_ADD_COLORS,               // Blend textures adding colors (alternative)
        BLEND_SUBTRACT_COLORS,          // Blend textures subtracting colors (alternative)
        BLEND_ALPHA_PREMULTIPLY,        // Blend premultiplied textures considering alpha
        BLEND_CUSTOM,                   // Blend textures using custom src/dst factors (use rlSetBlendFactors())
        BLEND_CUSTOM_SEPARATE           // Blend textures using custom rgb/alpha separate src/dst factors (use rlSetBlendFactorsSeparate())
    };

    enum class CullMode
    {
        None,
        Front,
        Back
    };

    enum class DepthCompare
    {
        Less,
        LessEqual,
        Equal,
        Greater,
        GreaterEqual,
        Always
    };

    struct RenderState
    {
        CullMode cullMode = CullMode::None;

        bool depthTest = true;
        bool depthWrite = true;
        DepthCompare depthCompare = DepthCompare::LessEqual;

        bool blend = false;
    };

    enum class RenderMode
    {
        WireframeShaded,
        Wireframe,
        Shaded
    };

    enum class ComputeAccess
    {
        Read,
        Write,
        ReadWrite
    };

    struct GpuReadBufferBinding
    {
        uint8_t slot = 0;
        const ComputeBuffer* buffer;
    };

    enum class TransientTopology
    {
        Points,
        Lines,
        Triangles
    };

    struct GpuVertexDrawDesc
    {
        const Material* material = nullptr;
        const ComputeBuffer* vertexBuffer = nullptr;
        uint32_t vertexCount = 0;
        const ComputeBuffer* indexBuffer = nullptr;
        uint32_t indexCount = 0;
        const ComputeBuffer* indirectBuffer = nullptr;
        TransientTopology topology = TransientTopology::Points;
    };

    struct TransientVertex
    {
        vec3 position = vec3(0.0f);
        vec2 uv = vec2(0.0f);
        vec3 normal = vec3(0.0f);
        Color color = WHITE;
    };

    struct TransientGeometry
    {
        TransientTopology topology = TransientTopology::Triangles;
        std::vector<TransientVertex> vertices;
        std::vector<uint16_t> indices;
    };

    struct TransientDrawDesc
    {
        mat4 mtx = mat4(1.0f);
        const Material* material = nullptr;
        TransientTopology topology = TransientTopology::Triangles;

        const TransientGeometry* geometry = nullptr; // cached/shared geometry if available

        std::vector<TransientVertex> vertices;       // fallback inline geometry
        std::vector<uint16_t> indices;               // fallback inline geometry
    };

    struct GpuInstanceDrawDesc
    {
        const Mesh* mesh = nullptr;
        const Material* material = nullptr;
        std::vector<GpuReadBufferBinding> readBuffers;
        uint32_t instanceCount = 0;
    };

    struct GpuBillboardDrawDesc
    {
        const Material* material = nullptr;
        const ComputeBuffer* positionBuffer = nullptr;
        uint32_t instanceCount = 0;
    };

    struct RenderItem
    {
        const Mesh* mesh = nullptr;
        const Material* material = nullptr;
        mat4 transform = mat4(1.0f);
    };

    struct MeshDrawPacket
    {
        const Mesh* mesh = nullptr;
        const Material* material = nullptr;
        mat4 world = mat4(1.0f);
    };

    struct InstancedDrawPacket
    {
        const Mesh* mesh = nullptr;
        const Material* material = nullptr;
        const void* data = nullptr;
        uint32_t instanceCount = 0;
        uint16_t stride = 0;
    };

    struct InstanceData
    {
        vec3 position;
        vec3 rotation;
        vec3 scale;
        Color color;
    };

    struct InstanceItem
    {
        const Mesh* mesh         = nullptr;
        const Material* material = nullptr;

        size_t dataOffset        = 0;
        size_t byteCount         = 0;

        uint32_t count           = 0;
        uint16_t stride          = 0;
    };

}

