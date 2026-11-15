#ifndef AIKO_SKINNING_GLSL
#define AIKO_SKINNING_GLSL

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

mat4 aikoSkinMatrix()
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

    return totalWeight > 0.0
        ? skin / totalWeight
        : mat4(1.0);
}

#endif
