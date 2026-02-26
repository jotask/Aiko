$input a_position, a_texcoord0, a_color0, a_normal
$output v_texcoord0, v_color0, v_normal

#include "bgfx_shader.sh"

void main()
{
    v_texcoord0 = a_texcoord0;
    v_color0    = a_color0;
    v_normal = mul(u_model[0], vec4(a_normal.xyz, 0.0)).xyz;
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
}