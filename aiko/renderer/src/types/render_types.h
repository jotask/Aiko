#pragma once

#include <map>


#include "aiko_types.h"

namespace aiko
{

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

    struct InstanceData
    {
        vec3 position;
        Color color;
    };

}

