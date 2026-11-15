
#version 450
#extension GL_GOOGLE_include_directive : require

#include "aiko_graphics.glsl"

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord0;
layout(location = 3) in vec4 a_color0;

layout(location = 9) in uvec4 a_jointIndices;
layout(location = 10) in vec4 a_jointWeights;

layout(
    set = AIKO_GRAPHICS_GPU_READ_SET,
    binding = AIKO_GRAPHICS_SKIN_PALETTE_BINDING,
    std430
) readonly buffer SkinPalette
{
    mat4 u_skinMatrices[];
};

layout(location = 0) out vec2 v_texcoord0;
layout(location = 1) out vec4 v_color0;
layout(location = 2) out vec3 v_normal;
layout(location = 3) out vec3 v_worldPos;

void main()
{
    mat4 skin = mat4(0.0);
    float totalWeight = 0.0;

    for (int i = 0; i < 4; ++i)
    {
        float weight = a_jointWeights[i];

        if (weight <= 0.0)
            continue;

        skin += u_skinMatrices[a_jointIndices[i]] * weight;
        totalWeight += weight;
    }

    if (totalWeight > 0.0)
        skin /= totalWeight;
    else
        skin = mat4(1.0);

    vec4 skinnedPosition = skin * vec4(a_position, 1.0);
    vec3 skinnedNormal = mat3(skin) * a_normal;

    vec4 worldPosition = u_model * skinnedPosition;

    v_texcoord0 = a_texcoord0;
    v_color0 = a_color0;
    v_worldPos = worldPosition.xyz;
    v_normal = normalize(
        transpose(inverse(mat3(u_model))) * skinnedNormal
    );

    gl_PointSize = u_flags.w;
    gl_Position = aikoModelViewProj() * skinnedPosition;
}
