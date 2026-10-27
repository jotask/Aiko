#version 450
#extension GL_GOOGLE_include_directive : require

#include "aiko_graphics.glsl"

layout(location = 0) in vec3 a_position;

void main()
{
    gl_Position = aikoModelViewProj() * vec4(a_position, 1.0);
}
