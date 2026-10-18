#version 450
#extension GL_GOOGLE_include_directive : require

#include "aiko_descriptor_abi.glsl"

layout(location = 0) in vec2 v_uv;
layout(location = 0) out vec4 outColor;

layout(
    set = AIKO_GRAPHICS_MATERIAL_SET,
    binding = AIKO_MATERIAL_UBO_BINDING,
    std140
) uniform BloomUpsampleParams
{
    float u_filterScale;
};

layout(
    set = AIKO_GRAPHICS_MATERIAL_SET,
    binding = AIKO_MATERIAL_TEXTURE_BINDING
) uniform sampler2D u_texture;

layout(
    set = AIKO_GRAPHICS_MATERIAL_SET,
    binding = AIKO_MATERIAL_TEXTURE_BINDING + 1
) uniform sampler2D u_highTexture;

void main()
{
    vec2 texel = 1.0 / vec2(textureSize(u_texture, 0));
    vec2 d = texel * u_filterScale;

    // Tent-filter the lower-resolution accumulated glow.
    vec3 low = texture(u_texture, v_uv).rgb * 4.0;

    low += texture(u_texture, v_uv + vec2(-d.x, 0.0)).rgb * 2.0;
    low += texture(u_texture, v_uv + vec2( d.x, 0.0)).rgb * 2.0;
    low += texture(u_texture, v_uv + vec2(0.0, -d.y)).rgb * 2.0;
    low += texture(u_texture, v_uv + vec2(0.0,  d.y)).rgb * 2.0;

    low += texture(u_texture, v_uv + vec2(-d.x, -d.y)).rgb;
    low += texture(u_texture, v_uv + vec2( d.x, -d.y)).rgb;
    low += texture(u_texture, v_uv + vec2(-d.x,  d.y)).rgb;
    low += texture(u_texture, v_uv + vec2( d.x,  d.y)).rgb;

    low /= 16.0;

    vec3 high = texture(u_highTexture, v_uv).rgb;

    // Blend broad glow with the detail retained at this level.
    outColor = vec4(high + low * 0.5, 1.0);
}
