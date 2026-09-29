#version 450
#extension GL_GOOGLE_include_directive : require

#define AIKO_CUSTOM_MATERIAL_UBO
#include "aiko_graphics.glsl"

layout(
set = AIKO_GRAPHICS_MATERIAL_SET,
binding = AIKO_MATERIAL_UBO_BINDING
) uniform MaterialUbo
{
    vec4 u_baseColor;
    vec4 u_flags;

    float u_particleDiameter;
};

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord0;
layout(location = 3) in vec4 a_color0;

layout(std430, set = 2, binding = 7)
readonly buffer ParticlePositionBuffer
{
    vec4 particlePositions[];
};

layout(location = 0) out vec2 v_texcoord0;
layout(location = 1) out vec4 v_color0;
layout(location = 2) out vec3 v_normal;
layout(location = 3) out vec3 v_worldPos;

void main()
{
    uint instanceId = uint(gl_InstanceIndex);

    vec3 instancePosition = particlePositions[instanceId].xyz;

    vec3 localPosition = a_position * u_particleDiameter + instancePosition;
    vec4 worldPosition = u_model * vec4(localPosition, 1.0);

    v_texcoord0 = a_texcoord0;

    v_color0 = a_color0;
    v_worldPos = worldPosition.xyz;

    v_normal = (u_model * vec4(a_normal, 0.0)).xyz;

    gl_Position = aikoModelViewProj() * vec4(localPosition, 1.0);
}