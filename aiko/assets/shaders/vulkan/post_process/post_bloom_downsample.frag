#version 450
#extension GL_GOOGLE_include_directive : require

#include "aiko_descriptor_abi.glsl"

layout(location = 0) in vec2 v_uv;
layout(location = 0) out vec4 outColor;

layout(
    set = AIKO_GRAPHICS_MATERIAL_SET,
    binding = AIKO_MATERIAL_UBO_BINDING,
    std140
) uniform BloomDownsampleParams
{
    float u_filterScale;
};

layout(
    set = AIKO_GRAPHICS_MATERIAL_SET,
    binding = AIKO_MATERIAL_TEXTURE_BINDING
) uniform sampler2D u_texture;

void main()
{
    vec2 texel = 1.0 / vec2(textureSize(u_texture, 0));
    vec2 d = texel * u_filterScale;

    vec3 result = texture(u_texture, v_uv).rgb * 0.5;

    result += texture(u_texture, v_uv + vec2(-d.x, -d.y)).rgb * 0.125;
    result += texture(u_texture, v_uv + vec2( d.x, -d.y)).rgb * 0.125;
    result += texture(u_texture, v_uv + vec2(-d.x,  d.y)).rgb * 0.125;
    result += texture(u_texture, v_uv + vec2( d.x,  d.y)).rgb * 0.125;

    outColor = vec4(result, 1.0);
}
