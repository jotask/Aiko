#version 450
#extension GL_GOOGLE_include_directive : require

#define AIKO_MATERIAL_EXTRA_FIELDS \
    vec4 u_atmosphereParams; \
    vec4 u_sunsetColor;

#include "aiko_graphics.glsl"

layout(location = 2) in vec3 v_normal;
layout(location = 3) in vec3 v_worldPos;

layout(location = 0) out vec4 outColor;

// u_atmosphereParams:
// x = opacity/intensity
// y = rim falloff exponent
// z = sunset color strength
// w = reserved

void main()
{
    vec3 N = normalize(v_normal);

    vec3 toCamera = u_cameraPos.xyz - v_worldPos;
    float viewLengthSquared = dot(toCamera, toCamera);

    if (viewLengthSquared <= 1e-8)
    {
        outColor = vec4(0.0);
        return;
    }

    vec3 V = toCamera * inversesqrt(viewLengthSquared);

    // This first version is designed for cameras outside
    // the atmosphere, viewing its outer surface.
    float viewCos = dot(N, V);

    if (viewCos <= 0.0)
    {
        outColor = vec4(0.0);
        return;
    }

    // Stronger near the apparent horizon.
    float rim = pow(
        1.0 - clamp(viewCos, 0.0, 1.0),
        max(u_atmosphereParams.y, 1.0)
    );

    // Fade at the shell's outer silhouette to avoid a
    // hard, bright edge.
    rim *= smoothstep(0.0, 0.16, viewCos);

    // Find the first directional light.
    vec3 toSun = vec3(0.0);
    vec3 sunColor = vec3(1.0);
    float sunIntensity = 0.0;
    bool hasSun = false;

    int lightCount = clamp(int(u_lightCount.x), 0, MAX_LIGHTS);

    for (int i = 0; i < lightCount; ++i)
    {
        if (int(u_lightType[i].x) != 0)
        {
            continue;
        }

        vec3 direction = -u_lightDir[i].xyz;
        float lengthSquared = dot(direction, direction);

        if (lengthSquared <= 1e-8)
        {
            continue;
        }

        toSun = direction * inversesqrt(lengthSquared);
        sunColor = max(u_lightColorInt[i].rgb, vec3(0.0));
        sunIntensity = max(u_lightColorInt[i].w, 0.0);
        hasSun = true;
        break;
    }

    if (!hasSun || sunIntensity <= 0.0)
    {
        outColor = vec4(0.0);
        return;
    }

    float NdotL = dot(N, toSun);

    // Daylight falls smoothly across the terminator.
    float daylight = smoothstep(-0.20, 0.35, NdotL);

    // Warm scattering around sunrise and sunset.
    float twilight =
        1.0 - smoothstep(0.0, 0.30, abs(NdotL));

    twilight *= smoothstep(-0.20, 0.05, NdotL);

    float sunsetMix = clamp(
        twilight * u_atmosphereParams.z,
        0.0,
        1.0
    );

    vec3 atmosphereColor = mix( u_baseColor.rgb, u_sunsetColor.rgb, sunsetMix);

    atmosphereColor *= sunColor;

    // Very faint residual glow on the night side.
    float illumination = mix(0.02, 1.0, daylight);

    float opacity = u_baseColor.a * max(u_atmosphereParams.x, 0.0) * rim * illumination * sunIntensity;

    opacity = clamp(opacity, 0.0, 0.85);

    outColor = vec4(atmosphereColor, opacity);
}
