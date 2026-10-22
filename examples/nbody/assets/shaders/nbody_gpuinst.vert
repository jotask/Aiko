#version 450
#extension GL_GOOGLE_include_directive : require

#include "aiko_graphics.glsl"

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord0;
layout(location = 3) in vec4 a_color0;

layout(std430, set = AIKO_GRAPHICS_GPU_READ_SET, binding = 7
) readonly buffer NBodyPositions
{
    vec4 u_posMass[];
};

layout(location = 0) out vec2 v_texcoord0;
layout(location = 1) out vec4 v_color0;
layout(location = 2) out vec3 v_normal;
layout(location = 3) out vec3 v_worldPos;

void main()
{
    vec3 instancePosition = u_posMass[gl_InstanceIndex].xyz;

    float scale = 0.05;

    vec3 localPosition = a_position * scale;
    vec3 worldPosition = instancePosition + localPosition;

    v_texcoord0 = a_texcoord0;
    v_color0 = vec4(1.0);
    v_normal = a_normal;
    v_worldPos = worldPosition;

    gl_Position = aikoViewProj() * vec4(worldPosition, 1.0);

}
