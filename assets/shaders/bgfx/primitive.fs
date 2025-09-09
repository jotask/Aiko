$input v_texcoord0, v_color0

/*
 * Copyright 2011-2025 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx/blob/master/LICENSE
 */

#include "../common/common.sh"

uniform vec4 u_color;
uniform vec4 u_border;
uniform vec4 u_border_thickness;

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
    gl_FragColor = v_color0 * u_color;
}
