#version 450

#include "aiko_descriptor_abi.glsl"

layout(location = 0) in vec2 v_uv;

layout(set = AIKO_GRAPHICS_MATERIAL_SET, binding = AIKO_MATERIAL_UBO_BINDING, std140) uniform ShaderToyMaterial
{
    vec4 u_tint;
};

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(v_uv, 0.0, 1.0) * u_tint;
}
