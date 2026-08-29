#ifndef AIKO_COMPUTE_GLSL
#define AIKO_COMPUTE_GLSL

#extension GL_GOOGLE_include_directive : require

#include "aiko_constants.glsl"
#include "aiko_descriptor_abi.glsl"

layout(
    std140,
    set = AIKO_COMPUTE_SET,
    binding = AIKO_COMPUTE_FRAME_BINDING
) uniform AikoComputeFrame
{
    mat4 aiko_view;
    mat4 aiko_projection;
    mat4 aiko_viewProj;

    vec4 aiko_cameraPos;
    vec4 aiko_time;
};

#endif