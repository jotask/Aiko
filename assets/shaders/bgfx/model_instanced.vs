$input a_position, a_texcoord0, a_color0, a_normal
$input i_data0, i_data1, i_data2, i_data3
$output v_texcoord0, v_color0, v_normal, v_worldPos

#include "bgfx_shader.sh"

void main()
{

    // instance data
    vec3 position   = i_data0.xyz;
    vec3 rotation   = i_data1.xyz;
    vec3 scale      = i_data2.xyz;
    vec4 instColor  = i_data3;

    // build world position (simple instancing: translate + optional scale)
    vec3 worldPos = a_position * scale + position;

    v_texcoord0 = a_texcoord0;
    v_color0    = instColor;         // instance color overrides mesh vertex color
    v_worldPos  = worldPos;

    // normals: since we don't rotate, keep as-is
    // (if you add rotation later, this must change)
    v_normal = a_normal.xyz;

    // clip position (IMPORTANT)
    // If your bgfx_shader.sh provides u_viewProj, use this:
    gl_Position = mul(u_modelViewProj, vec4(worldPos, 1.0));
}