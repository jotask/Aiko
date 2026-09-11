#version 450
#extension GL_GOOGLE_include_directive : require

#include "aiko_graphics.glsl"

layout(location = 0) in vec4 a_position;

layout(location = 0) out vec2 v_texcoord0;
layout(location = 1) out vec4 v_color0;
layout(location = 2) out vec3 v_normal;
layout(location = 3) out vec3 v_worldPos;

void main()
{
    vec4 worldPosition = u_model * vec4(a_position.xyz, 1.0);

    v_texcoord0 = vec2(0.0);
    v_color0 = vec4(1.0);
    v_normal = vec3(0.0, 0.0, 1.0);
    v_worldPos = worldPosition.xyz;

    gl_Position = aikoModelViewProj() * vec4(a_position.xyz, 1.0);

    gl_PointSize = AIKO_GL_POINT_SIZE;
}
