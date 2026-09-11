#version 450
#extension GL_GOOGLE_include_directive : require

#include "aiko_graphics.glsl"

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in vec4 a_color;

layout(location = 0) out vec3 v_localPosition;

void main()
{
    v_localPosition = a_position;
    gl_Position = u_modelViewProj * vec4(a_position, 1.0);
}
