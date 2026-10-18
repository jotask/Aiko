#version 450
#extension GL_GOOGLE_include_directive : require

#include "aiko_graphics.glsl"
#include "aiko_model_lighting.glsl"

layout(location = 0) in vec2 v_texcoord0;
layout(location = 1) in vec4 v_color0;
layout(location = 2) in vec3 v_normal;
layout(location = 3) in vec3 v_worldPos;

layout(location = 0) out vec4 outColor;

void main()
{
    vec4 base = u_baseColor;

    if (u_flags.x > 0.5)
    {
        base *= texture(u_texture, v_texcoord0);
    }

    if (u_flags.y > 0.5)
    {
        base *= v_color0;
    }

    if (u_flags.z > 0.5)
    {
        base.rgb = aikoShadeModel(v_worldPos, v_normal, base.rgb);
    }

    outColor = base;
}
