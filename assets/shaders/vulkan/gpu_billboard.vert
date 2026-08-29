#version 450
#extension GL_GOOGLE_include_directive : require

#include "aiko_graphics.glsl"

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord0;
layout(location = 3) in vec4 a_color0;

layout(std430, set = AIKO_GRAPHICS_GPU_READ_SET, binding = 7) readonly buffer BillboardPositionBuffer
{
    vec4 u_pos[];
};

layout(location = 0) out vec2 v_texcoord0;
layout(location = 1) out vec4 v_color0;
layout(location = 2) out vec3 v_normal;
layout(location = 3) out vec3 v_worldPos;

void main()
{
    uint iid = uint(gl_InstanceIndex);

    vec3 center = u_pos[iid].xyz;
    float size = u_billboardParams.x;

    vec3 cameraRight = vec3(u_view[0].x, u_view[0].y, u_view[0].z);
    vec3 cameraUp = vec3(u_view[1].x, u_view[1].y, u_view[1].z);

    vec2 quad = a_position.xy;

    vec3 worldPos = center
        + cameraRight * (quad.x * size)
        + cameraUp * (quad.y * size);

    v_texcoord0 = a_texcoord0;
    v_color0 = a_color0;
    v_worldPos = worldPos;
    v_normal = vec3(0.0, 0.0, 1.0);

    gl_Position = aikoViewProj() * vec4(worldPos, 1.0);
}
