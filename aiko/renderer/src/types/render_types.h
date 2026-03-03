#pragma once

#include <aiko_types.h>
#include <math/math_vector.h>
#include "types/color.h"

namespace aiko
{

    class ComputeBuffer;

    using ViewId = uint16_t;

    constexpr ViewId COMPUTE_VIEW = 0;
    constexpr ViewId COMPUTE_DRAW = 1;
    constexpr ViewId SCENE_VIEW = 2;
    constexpr ViewId SCREEN_VIEW = 3;
    constexpr ViewId IMGUI_VIEW = 4;

    constexpr uint16_t MAX_LIGHTS = 8;

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

    class Rectangle
    {
    public:
        Rectangle(float x, float y, float width, float height)
            : x(x), y(y), width(width), height(height) { };
        float x;
        float y;
        float width;
        float height;
    };

    class Mesh;
    class Material;

    struct GpuInstanceDrawDesc
    {
        ViewId viewId = 0;
        const Mesh* mesh = nullptr;
        const Material* material = nullptr;

        // GPU buffer containing instance records (format must match shader)
        const ComputeBuffer* instanceBuffer = nullptr;

        uint32_t instanceCount = 0;
    };

    struct RenderItem
    {
        const Mesh* mesh = nullptr;
        const Material* material = nullptr;
        mat4 transform = mat4(1.0f);
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
        const Mesh* mesh            = nullptr;
        const Material* material    = nullptr;
        std::vector<uint8_t> data;
        uint32_t count              = 0;
        uint16_t stride             = 0;
    };

}

