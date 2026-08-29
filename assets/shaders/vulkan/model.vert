#version 450
#extension GL_GOOGLE_include_directive : require

#include "aiko_graphics.glsl"

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord0;
layout(location = 3) in vec4 a_color0;

layout(location = 0) out vec2 v_texcoord0;
layout(location = 1) out vec4 v_color0;
layout(location = 2) out vec3 v_normal;
layout(location = 3) out vec3 v_worldPos;

void main()
{
    v_texcoord0 = a_texcoord0;
    v_color0 = a_color0;

    vec4 wpos = u_model * vec4(a_position, 1.0);
    v_worldPos = wpos.xyz;
    v_normal = (u_model * vec4(a_normal.xyz, 0.0)).xyz;

    gl_PointSize = AIKO_GL_POINT_SIZE;

    gl_Position = aikoModelViewProj() * vec4(a_position, 1.0);
}
