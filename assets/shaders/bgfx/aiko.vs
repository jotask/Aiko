#include "varying.def.sc"

attribute vec3 aPos;
attribute vec2 aTexCoord;
attribute vec3 aColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    // assign attributes to varyings
    v_texcoord0 = aTexCoord;
    v_color0    = aColor;
}
