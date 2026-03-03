$input a_position, a_texcoord0, a_color0, a_normal
$output v_texcoord0, v_color0, v_normal, v_worldPos

#include "bgfx_compute.sh"
#include "bgfx_shader.sh"

uniform vec4 u_params;

void main()
{

    v_texcoord0 = a_texcoord0;
    v_color0    = a_color0;

    BUFFER_RO(u_pos, vec4, 7);

    uint iid = uint(gl_InstanceID);

    uint count = uint(u_params.y);
    if (iid >= count)
    {
        gl_Position = vec4(0.0);
        return;
    }

    vec3 instanceOffset = u_pos[iid].xyz;

    // u_model is float4x4 u_model[1] in your build, so use [0]
    vec4 worldPos = mul(u_model[0], vec4(a_position + instanceOffset, 1.0));

    v_worldPos = worldPos.xyz;

    // normal: also use u_model[0]
    v_normal = mul(u_model[0], vec4(a_normal.xyz, 0.0)).xyz;

    // guaranteed present in your generated code
    gl_Position = mul(u_modelViewProj, vec4(a_position + instanceOffset, 1.0));
}