$input v_texcoord0, v_color0

/*
 * Copyright 2011-2025 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx/blob/master/LICENSE
 */

#include "../common/common.sh"

SAMPLER2D(u_texture, 0);

void main()
{
    vec4 texColor = texture2D(u_texture, v_texcoord0);
    gl_FragColor = texColor * v_color0; // modulate with vertex color
}
