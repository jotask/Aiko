$input a_position, a_texcoord0
$output v_texcoord0

/*
 * Copyright 2011-2025 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx/blob/master/LICENSE
 */

#include "../common/common.sh"

void main()
{
    v_texcoord0 = a_texcoord0;
    gl_Position = vec4(a_position, 1.0);
}
