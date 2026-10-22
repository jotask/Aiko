#version 450
#extension GL_GOOGLE_include_directive : require

#include "aiko_descriptor_abi.glsl"

layout(location = 0) in vec2 v_uv;
layout(location = 0) out vec4 outColor;

layout(
    set = AIKO_GRAPHICS_MATERIAL_SET,
    binding = 1
) uniform sampler2D u_texture;

layout(
    set = AIKO_GRAPHICS_MATERIAL_SET,
    binding = 2
) uniform sampler2D u_secondaryTexture;

void main()
{
    vec4 primary = texture(u_texture, v_uv);
    vec4 secondary = texture(u_secondaryTexture, v_uv);

    vec2 grid = floor(v_uv * 8.0);
    bool usePrimary = mod(grid.x + grid.y, 2.0) < 1.0;

    outColor = usePrimary ? primary : secondary;
}
