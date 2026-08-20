#version 450
#extension GL_GOOGLE_include_directive : require

#include "aiko_graphics.glsl"

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord0;
layout(location = 3) in vec4 a_color0;

layout(std430, set = 2, binding = 7) readonly buffer ParticlePositionBuffer
{
    vec4 u_pos[];
};

layout(std430, set = 2, binding = 8) readonly buffer ParticleLifeBuffer
{
    vec4 u_life[];
};

layout(location = 0) out vec2 v_texcoord0;
layout(location = 1) out vec4 v_color0;
layout(location = 2) out vec3 v_normal;
layout(location = 3) out vec3 v_worldPos;

void main()
{
    v_texcoord0 = a_texcoord0;

    uint iid = uint(gl_InstanceIndex);

    vec3 instanceOffset = u_pos[iid].xyz;
    vec4 life = u_life[iid];

    float remainingLife = life.x;
    float totalLife = max(life.y, 0.0000001);
    float life01 = 1.0 - clamp(remainingLife / totalLife, 0.0, 1.0);

    v_color0 = mix(u_particleStartColor, u_particleEndColor, life01);

    float startScale = u_particleSizeLife.x;
    float endScale = u_particleSizeLife.y;
    float particleScale = mix(startScale, endScale, life01);

    vec3 localPos = a_position * particleScale + instanceOffset;
    vec4 worldPos = u_model * vec4(localPos, 1.0);

    v_worldPos = worldPos.xyz;
    v_normal = (u_model * vec4(a_normal.xyz, 0.0)).xyz;

    gl_Position = aikoModelViewProj() * vec4(localPos, 1.0);
}
