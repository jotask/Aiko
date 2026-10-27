#version 450
#extension GL_GOOGLE_include_directive : require

#define AIKO_MATERIAL_EXTRA_FIELDS \
    vec4 u_pbrParams; \
    vec4 u_emissiveParams;

#include "aiko_graphics.glsl"
#include "aiko_color.glsl"
#include "aiko_shadow.glsl"

// Packed metallic/roughness texture.
// G = roughness, B = metallic.
layout(
    set = AIKO_GRAPHICS_MATERIAL_SET,
    binding = AIKO_MATERIAL_TEXTURE_BINDING + 1
) uniform sampler2D u_metallicRoughnessTexture;

layout(
    set = AIKO_GRAPHICS_MATERIAL_SET,
    binding = AIKO_MATERIAL_TEXTURE_BINDING + 2
) uniform sampler2D u_normalTexture;

// Ambient occlusion texture.
// R = occlusion factor.
layout(
    set = AIKO_GRAPHICS_MATERIAL_SET,
    binding = AIKO_MATERIAL_TEXTURE_BINDING + 3
) uniform sampler2D u_occlusionTexture;

layout(
    set = AIKO_GRAPHICS_MATERIAL_SET,
    binding = AIKO_MATERIAL_TEXTURE_BINDING + 4
) uniform sampler2D u_emissiveTexture;

layout(location = 0) in vec2 v_texcoord0;
layout(location = 1) in vec4 v_color0;
layout(location = 2) in vec3 v_normal;
layout(location = 3) in vec3 v_worldPos;
layout(location = 4) in vec4 v_tangent;

layout(location = 0) out vec4 outColor;

const float PI = 3.14159265358979323846;

// GGX / Trowbridge-Reitz normal distribution.
float distributionGGX(vec3 N, vec3 H, float roughness)
{
    float r = max(roughness, 0.045);
    float a = r * r;
    float a2 = a * a;

    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float d = NdotH2 * (a2 - 1.0) + 1.0;

    return a2 / max(PI * d * d, 1e-7);
}

// Schlick-GGX geometry approximation for direct lighting.
float geometrySchlickGGX(float NdotX, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    return NdotX / max(
        NdotX * (1.0 - k) + k,
        1e-5
    );
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);

    return geometrySchlickGGX(NdotV, roughness) * geometrySchlickGGX(NdotL, roughness);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (vec3(1.0) - F0) *
        pow(1.0 - clamp(cosTheta, 0.0, 1.0), 5.0);
}

float pointAttenuation(float distance, float range)
{
    float x = clamp(1.0 - distance / range, 0.0, 1.0);
    return x * x;
}

float spotAttenuation(vec3 L, vec3 direction, float innerCos, float outerCos)
{
    float cd = dot(-L, normalize(direction));

    return clamp(
        (cd - outerCos) / max(innerCos - outerCos, 1e-5),
        0.0,
        1.0
    );
}

float projectedShadowVisibility(vec3 worldPos)
{
    if (u_shadowParams.x < 0.5)
    {
        return 1.0;
    }

    vec4 lightClip = u_lightViewProjection * vec4(worldPos, 1.0);

    if (lightClip.w <= 0.0)
    {
        return 1.0;
    }

    vec3 projected = lightClip.xyz / lightClip.w;

    // Vulkan NDC depth is [0, 1].
    // X and Y are converted to texture coordinates.
    vec2 uv = projected.xy * 0.5 + 0.5;

    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0 || projected.z < 0.0 || projected.z > 1.0)
    {
        return 1.0;
    }

    float storedDepth = texture(u_shadowMap, uv).r;

    const float bias = 0.001;

    return projected.z - bias <= storedDepth ? 1.0 : 0.0;
}

vec3 evaluatePBRLight(int index, vec3 P, vec3 N, vec3 V, vec3 albedo, float metallic, float roughness)
{
    int type = int(u_lightType[index].x);

    vec3 lightPosition = u_lightPosRange[index].xyz;
    float range = u_lightPosRange[index].w;

    vec3 lightDirection = u_lightDir[index].xyz;
    vec3 lightColor = aikoSrgbToLinear(u_lightColorInt[index].rgb);
    float intensity = u_lightColorInt[index].w;

    vec3 L;
    float attenuation = 1.0;

    if (type == 0)
    {
        L = normalize(-lightDirection);
    }
    else
    {
        vec3 toLight = lightPosition - P;
        float distance = length(toLight);

        if (range <= 0.0 || distance >= range)
        {
            return vec3(0.0);
        }

        L = toLight / max(distance, 1e-5);
        attenuation = pointAttenuation(distance, range);

        if (type == 2)
        {
            attenuation *= spotAttenuation(L, lightDirection, u_lightSpotCos[index].x, u_lightSpotCos[index].y);
        }
    }

    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);

    if (NdotL <= 0.0 || NdotV <= 0.0)
    {
        return vec3(0.0);
    }

    vec3 halfway = V + L;
    float halfwayLength2 = dot(halfway, halfway);

    if (halfwayLength2 <= 1e-8)
    {
        return vec3(0.0);
    }

    vec3 H = halfway * inversesqrt(halfwayLength2);

    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    float D = distributionGGX(N, H, roughness);
    float G = geometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = D * G * F;
    float denominator = max(4.0 * NdotV * NdotL, 1e-5);

    vec3 specular = numerator / denominator;

    // Energy-conserving diffuse contribution.
    vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);
    vec3 diffuse = kD * albedo / PI;

    vec3 radiance = lightColor * intensity * attenuation;

    return (diffuse + specular) * radiance * NdotL;
}

vec3 resolvePBRNormal(vec3 interpolatedNormal)
{
    vec3 N = normalize(interpolatedNormal);

    // No normal map assigned, or the texture is still loading.
    if (u_pbrParams.z < 0.5)
    {
        return N;
    }

    // Orthogonalize the tangent against the interpolated normal.
    vec3 T = v_tangent.xyz - N * dot(N, v_tangent.xyz);

    float tangentLength2 = dot(T, T);

    // This mesh has no usable tangent basis.
    if (tangentLength2 <= 1e-8)
    {
        return N;
    }

    T *= inversesqrt(tangentLength2);

    float handedness = v_tangent.w < 0.0 ? -1.0 : 1.0;

    vec3 B = cross(N, T) * handedness;

    // Tangent-space normal maps contain signed vector directions
    // encoded into the unsigned [0, 1] RGB range.
    vec3 mappedNormal = texture(u_normalTexture, v_texcoord0).rgb * 2.0 - 1.0;

    float mappedLength2 = dot(mappedNormal, mappedNormal);

    if (mappedLength2 <= 1e-8)
    {
        return N;
    }

    mappedNormal *= inversesqrt(mappedLength2);

    return normalize(
        T * mappedNormal.x +
        B * mappedNormal.y +
        N * mappedNormal.z
    );
}

vec3 shadePBR(vec3 P, vec3 normal, vec3 albedo)
{
    vec3 N = resolvePBRNormal(normal);

    vec3 toCamera = u_cameraPos.xyz - P;
    float distance2 = dot(toCamera, toCamera);

    vec4 mrSample = texture(u_metallicRoughnessTexture, v_texcoord0);

    float metallic = clamp(u_pbrParams.x * mrSample.b, 0.0, 1.0);

    float roughness = clamp(u_pbrParams.y * mrSample.g, 0.0, 1.0);

    // Ambient occlusion affects indirect lighting, not direct lights.
    float occlusion = clamp(texture(u_occlusionTexture, v_texcoord0).r, 0.0, 1.0);

    // Approximation until environment lighting / IBL is added.
    vec3 ambientColor = aikoSrgbToLinear(u_ambientColor.rgb);

    vec3 result = albedo * (1.0 - metallic) * ambientColor * u_ambientIntensity.x * occlusion;

    if (distance2 <= 1e-8)
    {
        return result;
    }

    vec3 V = toCamera * inversesqrt(distance2);

    int count = min(int(u_lightCount.x), MAX_LIGHTS);

    for (int i = 0; i < count; ++i)
    {
        vec3 contribution = evaluatePBRLight(
            i, P, N, V, albedo, metallic, roughness
        );

        if (u_shadowParams.x > 0.5 &&
            i == int(u_shadowParams.y))
        {
            contribution *= projectedShadowVisibility(P);
        }

        result += contribution;
    }

    return result;
}

void main()
{
    vec4 base = vec4( aikoSrgbToLinear(u_baseColor.rgb), u_baseColor.a);

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
        base.rgb = shadePBR(v_worldPos, v_normal, base.rgb);
    }

    vec3 emissiveColor = aikoSrgbToLinear(u_emissiveParams.rgb);

    vec3 emissiveTexture = texture(u_emissiveTexture, v_texcoord0).rgb;

    base.rgb += emissiveColor * emissiveTexture * u_emissiveParams.a;

    outColor = base;

}
