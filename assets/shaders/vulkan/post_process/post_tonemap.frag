#version 450
#extension GL_GOOGLE_include_directive : require

#include "aiko_descriptor_abi.glsl"

layout(location = 0) in vec2 v_uv;
layout(location = 0) out vec4 outColor;

layout(
    set = AIKO_GRAPHICS_MATERIAL_SET,
    binding = AIKO_MATERIAL_UBO_BINDING,
    std140
) uniform ToneMappingParams
{
    float u_exposure;
};

layout(
    set = AIKO_GRAPHICS_MATERIAL_SET,
    binding = AIKO_MATERIAL_TEXTURE_BINDING
) uniform sampler2D u_texture;

vec3 acesFitted(vec3 color)
{
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;

    return clamp(
        (color * (a * color + b)) /
        (color * (c * color + d) + e),
        0.0,
        1.0
    );
}

void main()
{
    vec3 hdrColor = texture(u_texture, v_uv).rgb;

    hdrColor = max(hdrColor, vec3(0.0));
    hdrColor *= u_exposure;

    vec3 mappedColor = acesFitted(hdrColor);

    // Display-linear output.
    // Do not manually apply gamma here.
    outColor = vec4(mappedColor, 1.0);
}
