#version 450
#extension GL_GOOGLE_include_directive : require

#include "aiko_graphics.glsl"

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord0;
layout(location = 3) in vec4 a_color0;
layout(location = 4) in vec4 i_data0;
layout(location = 5) in vec4 i_data1;
layout(location = 6) in vec4 i_data2;
layout(location = 7) in vec4 i_data3;

layout(location = 0) out vec2 v_texcoord0;
layout(location = 1) out vec4 v_color0;
layout(location = 2) out vec3 v_normal;
layout(location = 3) out vec3 v_worldPos;

void main()
{
    vec3 position = i_data0.xyz;
    vec3 rotation = i_data1.xyz;
    vec3 scale = i_data2.xyz;
    vec4 instColor = i_data3;

    vec3 worldPos = a_position * scale + position;

    v_texcoord0 = a_texcoord0;
    v_color0 = instColor;
    v_worldPos = worldPos;
    v_normal = a_normal.xyz;

    gl_Position = aikoModelViewProj() * vec4(worldPos, 1.0);
}
