#version 450
#extension GL_GOOGLE_include_directive : require

#include "aiko_graphics.glsl"
#include "aiko_skinning.glsl"

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord0;
layout(location = 3) in vec4 a_color0;

struct AikoMorphDelta
{
    vec4 position;
    vec4 normal;
};

// GPU layout matches VulkanMorphDelta (32 bytes per target/vertex entry).
// Target-major order: [targetIndex * vertexCount + vertexIndex].
layout(
    set = AIKO_GRAPHICS_GPU_READ_SET,
    binding = AIKO_GRAPHICS_MORPH_DELTAS_BINDING,
    std430
) readonly buffer AikoMorphDeltas
{
    AikoMorphDelta u_morphDeltas[];
};

layout(
    set = AIKO_GRAPHICS_GPU_READ_SET,
    binding = AIKO_GRAPHICS_MORPH_WEIGHTS_BINDING,
    std430
) readonly buffer AikoMorphWeights
{
    float u_morphWeights[];
};

layout(location = 0) out vec2 v_texcoord0;
layout(location = 1) out vec4 v_color0;
layout(location = 2) out vec3 v_normal;
layout(location = 3) out vec3 v_worldPos;

void main()
{
    vec3 morphedPosition = a_position;
    vec3 morphedNormal = a_normal;

    // Descriptor ranges determine the imported target and vertex counts.
    uint targetCount = uint(u_morphWeights.length());
    if (targetCount > 0u)
    {
        uint vertexCount = uint(u_morphDeltas.length()) / targetCount;
        uint vertexIndex = uint(gl_VertexIndex);

        for (uint targetIndex = 0u; targetIndex < targetCount; ++targetIndex)
        {
            float weight = u_morphWeights[targetIndex];
            if (weight == 0.0)
                continue;

            uint index = targetIndex * vertexCount + vertexIndex;
            morphedPosition += weight * u_morphDeltas[index].position.xyz;
            morphedNormal += weight * u_morphDeltas[index].normal.xyz;
        }
    }

    // Morph in mesh/model space BEFORE applying the skeleton.
    mat4 skin = aikoSkinMatrix();
    vec4 skinnedPosition = skin * vec4(morphedPosition, 1.0);
    vec3 skinnedNormal = mat3(skin) * normalize(morphedNormal);

    vec4 worldPosition = u_model * skinnedPosition;
    v_texcoord0 = a_texcoord0;
    v_color0 = a_color0;
    v_worldPos = worldPosition.xyz;
    v_normal = normalize(transpose(inverse(mat3(u_model))) * skinnedNormal);

    gl_PointSize = u_flags.w;
    gl_Position = aikoModelViewProj() * skinnedPosition;
}
