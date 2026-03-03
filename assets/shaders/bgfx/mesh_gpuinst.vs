$input a_position, a_normal, a_texcoord0, a_color0
$output v_texcoord0, v_color0, v_normal, v_worldPos

#include "bgfx_shader.sh"
#include "bgfx_compute.sh"

// IMPORTANT: don't use stage 0 (reserved for textures)
BUFFER_RO(u_pos, vec4, 7);

void main()
{

    uint iid = uint(gl_InstanceID);

    vec4 ps = u_pos[iid]; // xyz = translation, w = scale
    vec3 worldPos = a_position * ps.w + ps.xyz;

    v_worldPos  = worldPos;
    v_texcoord0 = a_texcoord0;
    v_normal    = a_normal;
    v_color0    = vec4(1.0, 1.0, 1.0, 1.0);

    gl_Position = mul(u_viewProj, vec4(worldPos, 1.0));
}