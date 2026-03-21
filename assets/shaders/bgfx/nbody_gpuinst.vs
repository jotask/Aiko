$input a_position, a_texcoord0, a_color0, a_normal
$output v_texcoord0, v_color0, v_normal, v_worldPos

#include "bgfx_shader.sh"
#include "bgfx_compute.sh"

// slot 7 = positionMass buffer
BUFFER_RO(u_posMass, vec4, 7);

uniform vec4 u_nbodyRender; // x = render scale

void main()
{
    v_texcoord0 = a_texcoord0;

    uint iid = uint(gl_InstanceID);

    vec4 posMass = u_posMass[iid];
    vec3 instanceOffset = posMass.xyz;

    float particleScale = u_nbodyRender.x;
    vec3 localPos = a_position * particleScale + instanceOffset;

    vec4 worldPos = mul(u_model[0], vec4(localPos, 1.0));

    v_worldPos = worldPos.xyz;
    v_normal   = mul(u_model[0], vec4(a_normal.xyz, 0.0)).xyz;
    v_color0   = a_color0;

    gl_Position = mul(u_modelViewProj, vec4(localPos, 1.0));
}