$input v_texcoord0, v_color0

/*
 * Copyright 2011-2025 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx/blob/master/LICENSE
 */

#include "../common/common.sh"

void main()
{
    gl_FragColor = vec4(v_color0, 1.0);

    // Optional: mix textures
    // gl_FragColor = lerp(
    //     texture2D(texture1, v_texcoord0),
    //     texture2D(texture2, v_texcoord0),
    //     0.2
    // );
}
