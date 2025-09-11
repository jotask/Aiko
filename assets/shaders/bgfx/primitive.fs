$input v_texcoord0, v_color0, v_normal

/*
 * Copyright 2011-2025 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx/blob/master/LICENSE
 */

#include "../common/common.sh"

uniform vec4 u_color;
uniform vec4 u_border;
uniform vec4 u_border_thickness;

uniform vec4 u_ambient;   // ambient strength
uniform vec4 u_lightDir;   // xyz = light direction, w = unused
uniform vec4 u_lightColor; // light color/intensity

void main()
{

    const float border = u_border.x;
    const float border_thickness = u_border_thickness.x;

    if(border.x > 0.5f)
    {
        if( v_texcoord0.x > border_thickness && v_texcoord0.x < 1.0 - border_thickness &&
            v_texcoord0.y > border_thickness && v_texcoord0.y < 1.0 - border_thickness)
        {
            discard;
        }
    }

    // normalize light and normal
    vec3 N = normalize(v_normal.xyz);
    vec3 L = normalize(u_lightDir.xyz);

    // Lambert shading
    float NdotL = max(dot(N, L), 0.0);

    vec4 baseColor = v_color0 * u_color;

    // Combine base color with diffuse lighting
    vec4 shaded = baseColor * (u_ambient.x + NdotL * u_lightColor); // 0.2 = ambient

    gl_FragColor = shaded;

}
