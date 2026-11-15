#ifndef AIKO_MODEL_LIGHTING_GLSL
#define AIKO_MODEL_LIGHTING_GLSL

// Requires aiko_graphics.glsl.
#include "aiko_shadow.glsl"

float aikoPointAttenuation(float dist, float range)
{
    float x = clamp(1.0 - dist / range, 0.0, 1.0);
    return x * x;
}

float aikoSpotFactor(vec3 L, vec3 spotDir, float innerCos, float outerCos)
{
    float cd = dot(-L, normalize(spotDir));

    return clamp(
        (cd - outerCos) / max(innerCos - outerCos, 1e-5),
        0.0,
        1.0
    );
}

float aikoShadowVisibility(int index, vec3 P, vec3 N, vec3 L)
{
    float mode = u_shadowParams[index].x;

    if (mode < 0.5)
        return 1.0;

    float bias = max(
        u_shadowParams[index].y,
        u_shadowParams[index].z *
        (1.0 - max(dot(N, L), 0.0))
    );

    // Point light: cubemap shadow.
    if (mode > 1.5)
    {
        vec3 direction = P - u_lightPosRange[index].xyz;

        float majorDistance = max(
            abs(direction.x),
            max(abs(direction.y), abs(direction.z))
        );

        float nearPlane = u_pointShadowPlanes[index].x;
        float farPlane = u_pointShadowPlanes[index].y;

        if (majorDistance <= nearPlane ||
            majorDistance >= farPlane)
            return 1.0;

        float referenceDepth =
            farPlane / (farPlane - nearPlane) -
            (farPlane * nearPlane) /
            ((farPlane - nearPlane) * majorDistance);

        float storedDepth = texture(u_pointShadowMaps[index], direction).r;

        return referenceDepth - bias <= storedDepth ? 1.0 : 0.0;
    }

    // Directional and spot lights.
    vec4 clip = u_lightViewProjection[index] * vec4(P, 1.0);

    if (clip.w <= 0.0)
        return 1.0;

    vec3 projected = clip.xyz / clip.w;
    vec2 uv = projected.xy * 0.5 + 0.5;

    if (uv.x < 0.0 || uv.x > 1.0 ||
        uv.y < 0.0 || uv.y > 1.0 ||
        projected.z < 0.0 || projected.z > 1.0)
        return 1.0;

    vec2 texelSize = 1.0 / vec2(textureSize(u_shadowMaps[index], 0));

    float visibility = 0.0;

    // 3x3 PCF.
    for (int y = -1; y <= 1; ++y)
    {
        for (int x = -1; x <= 1; ++x)
        {
            vec2 sampleUv = uv + vec2(x, y) * texelSize;

            float storedDepth = texture(u_shadowMaps[index], sampleUv).r;

            visibility += projected.z - bias <= storedDepth ? 1.0 : 0.0;
        }
    }

    return visibility / 9.0;
}

vec3 aikoApplyModelLight(int index, vec3 P, vec3 N, vec3 albedo)
{
    int type = int(u_lightType[index].x);

    vec3 lightPos = u_lightPosRange[index].xyz;
    float range = u_lightPosRange[index].w;

    vec3 lightDir = u_lightDir[index].xyz;
    vec3 lightColor = u_lightColorInt[index].rgb;
    float intensity = u_lightColorInt[index].w;

    float innerCos = u_lightSpotCos[index].x;
    float outerCos = u_lightSpotCos[index].y;

    vec3 L;
    float attenuation = 1.0;

    if (type == 0)
    {
        L = normalize(-lightDir);
    }
    else
    {
        vec3 toLight = lightPos - P;
        float dist = length(toLight);

        if (dist > range)
        {
            return vec3(0.0);
        }

        L = toLight / max(dist, 1e-5);
        attenuation = aikoPointAttenuation(dist, range);

        if (type == 2)
        {
            attenuation *= aikoSpotFactor(L, lightDir, innerCos, outerCos);
        }
    }

    float NdotL = max(dot(N, L), 0.0);

    float visibility = aikoShadowVisibility(index, P, N, L);

    vec3 diffuse = albedo * lightColor * intensity * NdotL * attenuation * visibility;

    float specularStrength = u_specularParams.x;
    float shininess = u_specularParams.y;

    if (specularStrength <= 0.0 || NdotL <= 0.0)
    {
        return diffuse;
    }

    vec3 toCamera = u_cameraPos.xyz - P;
    float viewLengthSquared = dot(toCamera, toCamera);

    if (viewLengthSquared <= 1e-8)
    {
        return diffuse;
    }

    vec3 V = toCamera * inversesqrt(viewLengthSquared);

    vec3 halfVector = L + V;
    float halfLengthSquared = dot(halfVector, halfVector);

    if (halfLengthSquared <= 1e-8)
    {
        return diffuse;
    }

    vec3 H = halfVector * inversesqrt(halfLengthSquared);

    float specularFactor = pow(max(dot(N, H), 0.0), max(shininess, 1.0));

    vec3 specular = lightColor * intensity * attenuation * specularStrength * specularFactor * visibility;

    return diffuse + specular;
}

vec3 aikoShadeModel(vec3 worldPosition, vec3 worldNormal, vec3 albedo)
{
    vec3 N = normalize(worldNormal);

    vec3 result = albedo * u_ambientColor.rgb * u_ambientIntensity.x;

    int count = int(u_lightCount.x);

    for (int i = 0; i < MAX_LIGHTS; ++i)
    {
        if (i >= count)
        {
            break;
        }

        result += aikoApplyModelLight(i, worldPosition, N, albedo);
    }

    return result;
}

#endif
