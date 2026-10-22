#version 450
#extension GL_GOOGLE_include_directive : require

#include "aiko_descriptor_abi.glsl"

layout(location = 0) in vec2 v_uv;
layout(location = 0) out vec4 outColor;

layout(
    set = AIKO_GRAPHICS_MATERIAL_SET,
    binding = AIKO_MATERIAL_UBO_BINDING,
    std140
) uniform DisplayParams
{
    float u_exposure;
};

layout(
    set = AIKO_GRAPHICS_MATERIAL_SET,
    binding = AIKO_MATERIAL_TEXTURE_BINDING
) uniform sampler2D u_texture;

void main()
{
    vec3 hdr = max(texture(u_texture, v_uv).rgb, vec3(0.0));

    // Preserve a material UBO required by Aiko's Vulkan
    // graphics material ABI.
    hdr *= u_exposure;

    // Reinhard tone mapping, without artistic ACES curve.
    vec3 mapped = hdr / (vec3(1.0) + hdr);

    outColor = vec4(mapped, 1.0);
}
