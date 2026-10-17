#version 450
#extension GL_GOOGLE_include_directive : require

#define AIKO_MATERIAL_EXTRA_FIELDS \
    vec4 u_planetCenterRadius; \
    vec4 u_atmosphereSettings; \
    vec4 u_rayleighScattering; \
    vec4 u_mieSettings;

#include "aiko_graphics.glsl"

layout(location = 3) in vec3 v_worldPos;
layout(location = 0) out vec4 outColor;

// u_planetCenterRadius: xyz = planet center, w = planet radius
// u_atmosphereSettings: x = atmosphere radius, y = Rayleigh scale height,
//                       z = Mie scale height, w = exposure
// u_rayleighScattering: rgb = Rayleigh scattering coefficients
// u_mieSettings: x = Mie scattering coefficient, y = anisotropy g,
//                z = opacity scale, w = maximum alpha

const float PI = 3.14159265359;
const int VIEW_STEPS = 16;
const int LIGHT_STEPS = 6;

// Coordinates are relative to the planet center.
bool raySphere(vec3 ro, vec3 rd, float radius, out vec2 hit)
{
    float b = dot(ro, rd);
    float c = dot(ro, ro) - radius * radius;
    float d = b * b - c;

    if (d < 0.0)
    {
        hit = vec2(0.0);
        return false;
    }

    float root = sqrt(max(d, 0.0));
    hit = vec2(-b - root, -b + root);
    return hit.y > 0.0;
}

vec2 atmosphericDensity(vec3 p, float groundRadius, vec2 scaleHeights)
{
    float altitude = max(length(p) - groundRadius, 0.0);
    return exp(-altitude / scaleHeights);
}

vec2 opticalDepthToSun(
    vec3 p,
    vec3 sunDir,
    float groundRadius,
    float atmosphereRadius,
    vec2 scaleHeights)
{
    vec2 atmosphereHit;
    if (!raySphere(p, sunDir, atmosphereRadius, atmosphereHit))
    {
        return vec2(0.0);
    }

    float distance = max(atmosphereHit.y, 0.0);
    float stepLength = distance / float(LIGHT_STEPS);
    vec2 depth = vec2(0.0);

    for (int i = 0; i < LIGHT_STEPS; ++i)
    {
        vec3 samplePos = p + sunDir * ((float(i) + 0.5) * stepLength);
        depth += atmosphericDensity(samplePos, groundRadius, scaleHeights) * stepLength;
    }

    return depth;
}

void main()
{
    outColor = vec4(0.0);

    float groundRadius = u_planetCenterRadius.w;
    float atmosphereRadius = u_atmosphereSettings.x;
    vec2 scaleHeights = u_atmosphereSettings.yz;

    if (groundRadius <= 0.0 || atmosphereRadius <= groundRadius ||
        min(scaleHeights.x, scaleHeights.y) <= 0.0)
    {
        return;
    }

    vec3 cameraPos = u_cameraPos.xyz;
    vec3 ro = cameraPos - u_planetCenterRadius.xyz;
    vec3 cameraToFragment = v_worldPos - cameraPos;
    float rayLengthSquared = dot(cameraToFragment, cameraToFragment);

    if (rayLengthSquared <= 1e-8)
    {
        return;
    }

    vec3 rd = cameraToFragment * inversesqrt(rayLengthSquared);

    vec2 atmosphereHit;
    if (!raySphere(ro, rd, atmosphereRadius, atmosphereHit))
    {
        return;
    }

    float start = max(atmosphereHit.x, 0.0);
    float stop = atmosphereHit.y;

    // Do not integrate through the solid planet.
    vec2 planetHit;
    if (raySphere(ro, rd, groundRadius, planetHit) && planetHit.x > 0.0)
    {
        stop = min(stop, planetHit.x);
    }

    if (stop <= start)
    {
        return;
    }

    // First directional light is the Sun.
    vec3 sunDir = vec3(0.0);
    vec3 sunColor = vec3(0.0);
    float sunIntensity = 0.0;
    bool foundSun = false;

    int count = clamp(int(u_lightCount.x), 0, MAX_LIGHTS);
    for (int i = 0; i < count; ++i)
    {
        if (int(u_lightType[i].x) != 0)
        {
            continue;
        }

        vec3 direction = -u_lightDir[i].xyz;
        float directionLengthSquared = dot(direction, direction);
        if (directionLengthSquared <= 1e-8)
        {
            continue;
        }

        sunDir = direction * inversesqrt(directionLengthSquared);
        sunColor = max(u_lightColorInt[i].rgb, vec3(0.0));
        sunIntensity = max(u_lightColorInt[i].w, 0.0);
        foundSun = true;
        break;
    }

    if (!foundSun || sunIntensity <= 0.0)
    {
        return;
    }

    vec3 betaR = max(u_rayleighScattering.rgb, vec3(0.0));
    float betaM = max(u_mieSettings.x, 0.0);
    float g = clamp(u_mieSettings.y, -0.95, 0.95);
    float exposure = max(u_atmosphereSettings.w, 0.0);

    // Angle between incoming sunlight propagation and outgoing camera ray.
    float mu = clamp(dot(rd, sunDir), -1.0, 1.0);
    float phaseR = (3.0 / (16.0 * PI)) * (1.0 + mu * mu);
    float phaseM = (1.0 - g * g) /
                   (4.0 * PI * pow(max(1.0 + g * g - 2.0 * g * mu, 1e-4), 1.5));

    float stepLength = (stop - start) / float(VIEW_STEPS);
    vec2 viewDepth = vec2(0.0);
    vec3 sumR = vec3(0.0);
    vec3 sumM = vec3(0.0);

    for (int i = 0; i < VIEW_STEPS; ++i)
    {
        float t = start + (float(i) + 0.5) * stepLength;
        vec3 samplePos = ro + rd * t;
        vec2 density = atmosphericDensity(samplePos, groundRadius, scaleHeights);
        viewDepth += density * stepLength;

        // A planet intersection means this sample is in the planet's shadow.
        vec2 shadowHit;
        if (raySphere(samplePos, sunDir, groundRadius, shadowHit) && shadowHit.x > 0.0)
        {
            continue;
        }

        vec2 sunDepth = opticalDepthToSun(
            samplePos, sunDir, groundRadius, atmosphereRadius, scaleHeights);

        vec3 opticalDepth = betaR * (viewDepth.x + sunDepth.x) +
                            vec3(betaM * (viewDepth.y + sunDepth.y));
        vec3 transmittance = exp(-opticalDepth);

        sumR += transmittance * density.x * stepLength;
        sumM += transmittance * density.y * stepLength;
    }

    vec3 radiance = (sumR * betaR * phaseR + sumM * betaM * phaseM) *
                    sunColor * sunIntensity;

    // Tone map for an LDR, alpha-blended output.
    vec3 mapped = vec3(1.0) - exp(-max(radiance, vec3(0.0)) * exposure);
    mapped *= clamp(u_baseColor.rgb, vec3(0.0), vec3(1.0));
    mapped *= clamp(u_baseColor.a, 0.0, 1.0);

    float opacityScale = max(u_mieSettings.z, 0.0);
    float maxAlpha = clamp(u_mieSettings.w, 0.0, 1.0);
    float alpha = clamp(max(max(mapped.r, mapped.g), mapped.b) * opacityScale,
                        0.0, maxAlpha);

    if (alpha <= 1e-5)
    {
        return;
    }

    // Vulkan's current material blending is straight alpha (SRC_ALPHA).
    // Undo premultiplication so the blended contribution is ~mapped.
    vec3 straightColor = clamp(mapped / alpha, vec3(0.0), vec3(1.0));
    outColor = vec4(straightColor, alpha);
}
