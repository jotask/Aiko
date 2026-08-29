#version 450
#extension GL_GOOGLE_include_directive : require

#include "aiko_graphics.glsl"

layout(location = 0) in vec2 v_texcoord0;
layout(location = 1) in vec4 v_color0;
layout(location = 2) in vec3 v_normal;
layout(location = 3) in vec3 v_worldPos;

layout(location = 0) out vec4 outColor;

float attenPoint(float dist, float range)
{
    float x = clamp(1.0 - dist / range, 0.0, 1.0);
    return x * x;
}

float spotFactor(vec3 L, vec3 spotDir, float innerCos, float outerCos)
{
    float cd = dot(-L, normalize(spotDir));
    float t = clamp((cd - outerCos) / max(innerCos - outerCos, 1e-5), 0.0, 1.0);
    return t;
}

vec3 applyLight(
    int type,
    vec3 P,
    vec3 N,
    vec3 albedo,
    vec3 camPos,
    vec3 lightPos,
    float range,
    vec3 lightDir,
    vec3 lightColor,
    float intensity,
    float innerCos,
    float outerCos)
{
    vec3 L;
    float att = 1.0;

    if (type == 0)
    {
        L = normalize(-lightDir);
        att = 1.0;
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
        att = attenPoint(dist, range);

        if (type == 2)
        {
            float s = spotFactor(L, lightDir, innerCos, outerCos);
            att *= s;
        }
    }

    float NdotL = max(dot(N, L), 0.0);
    return albedo * (lightColor * intensity) * NdotL * att;
}

void main()
{
    vec4 base = u_baseColor;

    if (u_flags.x > 0.5)
    {
        vec4 tex = texture(u_texture, v_texcoord0);
        base *= tex;
    }

    if (u_flags.y > 0.5)
    {
        base *= v_color0;
    }

    if (u_flags.z > 0.5)
    {
        vec3 P = v_worldPos.xyz;
        vec3 N = normalize(v_normal.xyz);
        vec3 camPos = u_cameraPos.xyz;

        vec3 albedo = base.rgb;
        vec3 ambient = albedo * u_ambientColor.rgb * u_ambientIntensity.x;
        vec3 sum = ambient;

        int count = int(u_lightCount.x);
        for (int i = 0; i < MAX_LIGHTS; ++i)
        {
            if (i >= count)
            {
                break;
            }

            int type = int(u_lightType[i].x);
            vec3 lpos = u_lightPosRange[i].xyz;
            float range = u_lightPosRange[i].w;
            vec3 ldir = u_lightDir[i].xyz;
            vec3 lcol = u_lightColorInt[i].rgb;
            float lint = u_lightColorInt[i].w;
            float innerCos = u_lightSpotCos[i].x;
            float outerCos = u_lightSpotCos[i].y;

            sum += applyLight(type, P, N, albedo, camPos, lpos, range, ldir, lcol, lint, innerCos, outerCos);
        }

        base.rgb = sum;
    }

    outColor = base;
}
