#ifndef AIKO_COMPUTE_GLSL
#define AIKO_COMPUTE_GLSL

#define AIKO_MAX_COMPUTE_BUFFER_BINDINGS 4
#define AIKO_MAX_COMPUTE_IMAGE_BINDINGS 8

#define AIKO_COMPUTE_BUFFER_BINDING(slot) \
    slot

#define AIKO_COMPUTE_IMAGE_BINDING(slot) \
    (AIKO_MAX_COMPUTE_BUFFER_BINDINGS + slot)

#define AIKO_COMPUTE_FRAME_BINDING \
    (AIKO_MAX_COMPUTE_BUFFER_BINDINGS + \
     AIKO_MAX_COMPUTE_IMAGE_BINDINGS)

layout(
    std140,
    set = 0,
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