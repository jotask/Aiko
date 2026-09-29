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
    uint u_particleColorMode;
    float u_velocityColorScale;
    float u_pressureColorScale;
};

layout(std430, set = 2, binding = 8)
readonly buffer ParticleVelocityBuffer
{
    vec4 particleVelocities[];
};

layout(std430, set = 2, binding = 9)
readonly buffer ParticlePressureBuffer
{
    vec4 particlePressures[];
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

vec3 heatColor(float value)
{
    float t = clamp(value, 0.0, 1.0);

    vec3 blue = vec3(0.0, 0.2, 1.0);
    vec3 cyan = vec3(0.0, 1.0, 1.0);
    vec3 yellow = vec3(1.0, 1.0, 0.0);
    vec3 red = vec3(1.0, 0.0, 0.0);

    if (t < 0.33)
    {
        return mix(blue, cyan, t / 0.33);
    }

    if (t < 0.66)
    {
        return mix(cyan,  yellow,  (t - 0.33) / 0.33);
    }

    return mix(yellow,  red,  (t - 0.66) / 0.34);
}

void main()
{
    uint instanceId = uint(gl_InstanceIndex);

    vec3 instancePosition = particlePositions[instanceId].xyz;

    vec3 localPosition = a_position * u_particleDiameter + instancePosition;
    vec4 worldPosition = u_model * vec4(localPosition, 1.0);

    v_texcoord0 = a_texcoord0;

    vec4 particleColor = u_baseColor;

    if (u_particleColorMode == 1u)
    {
        float speed =length(particleVelocities[instanceId].xyz);

        float value = speed / max(u_velocityColorScale, 0.000001);

        particleColor = vec4(heatColor(value), 1.0);
    }
    else if (u_particleColorMode == 2u)
    {
        float pressure = particlePressures[instanceId].x;

        float value = pressure / max(u_pressureColorScale, 0.000001);

        particleColor = vec4(heatColor(value), 1.0);
    }

    v_color0 = particleColor;
    v_worldPos = worldPosition.xyz;

    v_normal = (u_model * vec4(a_normal, 0.0)).xyz;

    gl_Position = aikoModelViewProj() * vec4(localPosition, 1.0);
}