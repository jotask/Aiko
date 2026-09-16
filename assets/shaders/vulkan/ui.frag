#version 450
#extension GL_GOOGLE_include_directive : require

#include "aiko_graphics.glsl"

layout(location = 0) in vec4 v_color0;
layout(location = 1) in vec2 v_texcoord0;

layout(location = 0) out vec4 outColor;

void main()
{
    vec4 color = v_color0;

    if (u_flags.x != 0.0)
    {
        color *= texture(u_texture, v_texcoord0);
    }

    outColor = color;
}