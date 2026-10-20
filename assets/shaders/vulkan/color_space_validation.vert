#version 450
#extension GL_GOOGLE_include_directive : require

#define AIKO_CUSTOM_MATERIAL_UBO
#include "aiko_graphics.glsl"

layout(location = 0) in vec3 a_position;
layout(location = 2) in vec2 a_texcoord0;

layout(location = 0) out vec2 v_uv;

void main()
{
    v_uv = a_texcoord0;
    gl_Position = aikoModelViewProj() * vec4(a_position, 1.0);
}
