#version 450
#extension GL_GOOGLE_include_directive : require

#include "aiko_descriptor_abi.glsl"

layout(location = 0) in vec2 v_uv;
layout(location = 0) out vec4 outColor;

layout(
    set = AIKO_GRAPHICS_MATERIAL_SET,
    binding = AIKO_MATERIAL_UBO_BINDING,
    std140
) uniform BloomExtractParams
{
    float u_threshold;
};

layout(
    set = AIKO_GRAPHICS_MATERIAL_SET,
    binding = AIKO_MATERIAL_TEXTURE_BINDING
) uniform sampler2D u_texture;

void main()
{
    vec3 color = max(texture(u_texture, v_uv).rgb, vec3(0.0));

    float brightness = max(max(color.r, color.g), color.b);

    // Preserve HDR energy above the extraction threshold.
    vec3 bright = brightness > u_threshold
        ? color * ((brightness - u_threshold) / max(brightness, 1e-6))
        : vec3(0.0);

    outColor = vec4(bright, 1.0);
}
