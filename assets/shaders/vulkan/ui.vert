#version 450
#extension GL_GOOGLE_include_directive : require

#include "aiko_graphics.glsl"

layout(location = 0) in vec3 a_position;
layout(location = 3) in vec4 a_color0;

layout(location = 0) out vec4 v_color0;

void main()
{
    v_color0 = a_color0;
    gl_Position = aikoModelViewProj() * vec4(a_position, 1.0);
}
