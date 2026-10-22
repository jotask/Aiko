#version 450
#extension GL_GOOGLE_include_directive : require

#define AIKO_CUSTOM_MATERIAL_UBO
#include "aiko_graphics.glsl"
#include "ui_material.glsl"

layout(location = 0) in vec4 v_color0;
layout(location = 1) in vec2 v_texcoord0;

layout(location = 0) out vec4 outColor;

float roundedRectDistance(
    vec2 point,
    vec2 halfSize,
    float radius)
{
    vec2 q =
        abs(point) -
        (halfSize - vec2(radius));

    return
        length(max(q, vec2(0.0))) +
        min(max(q.x, q.y), 0.0) -
        radius;
}

void main()
{
    vec4 color = v_color0;

    if (u_flags.x != 0.0)
    {
        color *= texture(
            u_texture,
            v_texcoord0);
    }

    vec2 rectPosition =
        u_uiRect.xy;

    vec2 rectSize =
        u_uiRect.zw;

    vec2 halfSize =
        rectSize * 0.5;

    float radius =
        clamp(
            u_uiStyle.x,
            0.0,
            min(halfSize.x, halfSize.y));

    float borderThickness =
        max(u_uiStyle.y, 0.0);

    vec2 center =
        rectPosition + halfSize;

    vec2 point =
        gl_FragCoord.xy - center;

    float distance =
        roundedRectDistance(
            point,
            halfSize,
            radius);

    float antialias =
        max(fwidth(distance), 0.0001);

    float outerAlpha =
        1.0 -
        smoothstep(
            -antialias,
            antialias,
            distance);

    if (outerAlpha <= 0.0)
    {
        discard;
    }

    if (borderThickness > 0.0)
    {
        float innerRadius =
            max(
                radius - borderThickness,
                0.0);

        vec2 innerHalfSize =
            max(
                halfSize -
                    vec2(borderThickness),
                vec2(0.0));

        float innerDistance =
            roundedRectDistance(
                point,
                innerHalfSize,
                innerRadius);

        float innerAlpha =
            1.0 -
            smoothstep(
                -antialias,
                antialias,
                innerDistance);

        float borderMask =
            outerAlpha *
            (1.0 - innerAlpha);

        color =
            mix(
                color,
                u_uiBorderColor,
                borderMask);
    }

    color.a *= outerAlpha;

    outColor = color;
}
