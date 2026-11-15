#version 450
#extension GL_GOOGLE_include_directive : require

#include "aiko_graphics.glsl"
#include "aiko_skinning.glsl"

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
    mat4 skin = aikoSkinMatrix();

    vec4 skinnedPosition = skin * vec4(a_position, 1.0);
    vec3 skinnedNormal = mat3(skin) * a_normal;

    vec4 worldPosition = u_model * skinnedPosition;

    v_texcoord0 = a_texcoord0;
    v_color0 = a_color0;
    v_worldPos = worldPosition.xyz;
    v_normal = normalize(transpose(inverse(mat3(u_model))) * skinnedNormal);

    gl_PointSize = u_flags.w;
    gl_Position = aikoModelViewProj() * skinnedPosition;
}
