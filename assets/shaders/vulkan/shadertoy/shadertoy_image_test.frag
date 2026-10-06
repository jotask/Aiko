#version 450

#include "aiko_descriptor_abi.glsl"

layout(set = AIKO_GRAPHICS_MATERIAL_SET, binding = AIKO_MATERIAL_UBO_BINDING, std140) uniform ShaderToyUniforms
{
    vec3 iResolution;
    float iTime;
};

layout(location = 0) out vec4 outColor;

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = fragCoord / iResolution.xy;

    float blue = 0.5 + 0.5 * sin(iTime);

    fragColor = vec4(uv, blue, 1.0);
}

void main()
{
    mainImage(outColor, gl_FragCoord.xy);
}