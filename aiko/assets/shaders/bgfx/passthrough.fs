$input v_texcoord0

/*
 * Copyright 2011-2025 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx/blob/master/LICENSE
 */

#include "../common/common.sh"

SAMPLER2D(u_scene, 0);

void main()
{
    gl_FragColor = texture2D(u_scene, v_texcoord0);
}
