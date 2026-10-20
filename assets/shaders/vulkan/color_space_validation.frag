#version 450
#extension GL_GOOGLE_include_directive : require

#include "aiko_descriptor_abi.glsl"

layout(location = 0) in vec2 v_uv;
layout(location = 0) out vec4 outColor;

layout(
    set = AIKO_GRAPHICS_MATERIAL_SET,
    binding = AIKO_MATERIAL_UBO_BINDING,
    std140
) uniform ColorSpaceParams
{
    float u_expectedLinear;
    float u_expectedSrgb;
};

layout(
    set = AIKO_GRAPHICS_MATERIAL_SET,
    binding = AIKO_MATERIAL_TEXTURE_BINDING
) uniform sampler2D u_texture;

layout(
    set = AIKO_GRAPHICS_MATERIAL_SET,
    binding = AIKO_MATERIAL_TEXTURE_BINDING + 1
) uniform sampler2D u_secondaryTexture;

void main()
{
    // One asset, two GPU formats.
    float srgbSample = texture(u_texture, v_uv).r;
    float linearSample = texture(u_secondaryTexture, v_uv).r;

    bool linearOK = abs(linearSample - u_expectedLinear) < 0.01;
    bool srgbOK = abs(srgbSample - u_expectedSrgb) < 0.01;

    if (v_uv.x < 0.3333)
    {
        outColor = vec4(vec3(linearSample), 1.0);
    }
    else if (v_uv.x < 0.6667)
    {
        outColor = vec4(vec3(srgbSample), 1.0);
    }
    else
    {
        outColor = vec4(
            linearOK && srgbOK
                ? vec3(0.0, 1.0, 0.0)
                : vec3(1.0, 0.0, 0.0),
            1.0
        );
    }
}
