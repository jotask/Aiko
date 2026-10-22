#version 450
#extension GL_GOOGLE_include_directive : require

#include "aiko_descriptor_abi.glsl"

layout(location = 0) in vec2 v_uv;
layout(location = 0) out vec4 outColor;

layout(
    set = AIKO_GRAPHICS_MATERIAL_SET,
    binding = AIKO_MATERIAL_UBO_BINDING,
    std140
) uniform BloomCompositeParams
{
    float u_intensity;
};

layout(
    set = AIKO_GRAPHICS_MATERIAL_SET,
    binding = AIKO_MATERIAL_TEXTURE_BINDING
) uniform sampler2D u_texture;

layout(
    set = AIKO_GRAPHICS_MATERIAL_SET,
    binding = AIKO_MATERIAL_TEXTURE_BINDING + 1
) uniform sampler2D u_bloomTexture;

void main()
{
    vec3 scene = texture(u_texture, v_uv).rgb;
    vec3 bloom = texture(u_bloomTexture, v_uv).rgb;

    outColor = vec4(scene + bloom * u_intensity, 1.0);
}
