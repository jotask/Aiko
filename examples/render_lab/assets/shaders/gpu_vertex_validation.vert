#version 450
#extension GL_GOOGLE_include_directive : require

#include "aiko_graphics.glsl"

layout(location = 0) in vec4 a_position;

void main()
{
    gl_Position = u_modelViewProj * vec4(a_position.xyz, 1.0);
}
