#pragma once

namespace aiko
{

    // Thanks raylib

    // Shader uniform data type
    typedef enum {
        SHADER_UNIFORM_FLOAT = 0,       // Shader uniform type: float
        SHADER_UNIFORM_VEC2,            // Shader uniform type: vec2 (2 float)
        SHADER_UNIFORM_VEC3,            // Shader uniform type: vec3 (3 float)
        SHADER_UNIFORM_VEC4,            // Shader uniform type: vec4 (4 float)
        SHADER_UNIFORM_INT,             // Shader uniform type: int
        SHADER_UNIFORM_IVEC2,           // Shader uniform type: ivec2 (2 int)
        SHADER_UNIFORM_IVEC3,           // Shader uniform type: ivec3 (3 int)
        SHADER_UNIFORM_IVEC4,           // Shader uniform type: ivec4 (4 int)
        SHADER_UNIFORM_SAMPLER2D        // Shader uniform type: sampler2d
    } ShaderUniformDataType;

    typedef enum {
        BLEND_ALPHA = 0,                // Blend textures considering alpha (default)
        BLEND_ADDITIVE,                 // Blend textures adding colors
        BLEND_MULTIPLIED,               // Blend textures multiplying colors
        BLEND_ADD_COLORS,               // Blend textures adding colors (alternative)
        BLEND_SUBTRACT_COLORS,          // Blend textures subtracting colors (alternative)
        BLEND_ALPHA_PREMULTIPLY,        // Blend premultiplied textures considering alpha
        BLEND_CUSTOM,                   // Blend textures using custom src/dst factors (use rlSetBlendFactors())
        BLEND_CUSTOM_SEPARATE           // Blend textures using custom rgb/alpha separate src/dst factors (use rlSetBlendFactorsSeparate())
    } BlendMode;


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

}

