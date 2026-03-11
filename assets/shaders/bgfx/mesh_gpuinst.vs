$input a_position, a_texcoord0, a_color0, a_normal
$output v_texcoord0, v_color0, v_normal, v_worldPos

#include "bgfx_compute.sh"
#include "bgfx_shader.sh"

// Bound from C++ with setBuffer(7, ...)
BUFFER_RO(u_pos, vec4, 7);

void main()
{
    v_texcoord0 = a_texcoord0;
    v_color0    = a_color0;

    uint iid = uint(gl_InstanceID);

    // Particle position comes directly from compute buffer.
    vec3 instanceOffset = u_pos[iid].xyz;

    // Make each particle mesh smaller so cubes are visible as particles.
    vec3 localPos = a_position * 0.05 + instanceOffset;

    // Your GPU instanced draw currently submits identity transform,
    // but we still keep the standard model/world pipeline.
    vec4 worldPos = mul(u_model[0], vec4(localPos, 1.0));

    v_worldPos = worldPos.xyz;
    v_normal   = mul(u_model[0], vec4(a_normal.xyz, 0.0)).xyz;

    gl_Position = mul(u_modelViewProj, vec4(localPos, 1.0));
}