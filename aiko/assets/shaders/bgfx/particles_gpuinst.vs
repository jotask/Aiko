$input a_position, a_texcoord0, a_color0, a_normal
$output v_texcoord0, v_color0, v_normal, v_worldPos

#include "bgfx_compute.sh"
#include "bgfx_shader.sh"

// Bound from C++ with setBuffer(7, ...)
BUFFER_RO(u_pos, vec4, 7);

// Optional particle life data bound from C++ with setBuffer(8, ...)
BUFFER_RO(u_life, vec4, 8);

uniform vec4 u_particleSizeLife; // x : start scale, y : end scale
uniform vec4 u_particleStartColor;
uniform vec4 u_particleEndColor;

void main()
{
    v_texcoord0 = a_texcoord0;

    uint iid = uint(gl_InstanceID);

    // Particle position comes directly from compute buffer.
    vec3 instanceOffset = u_pos[iid].xyz;

    vec4 life = u_life[iid];

    float remainingLife = life.x;
    float totalLife = max(life.y, 0.0000001f);

    float life01 = 1.0f - clamp(remainingLife / totalLife, 0.0, 1.0);

    // Color mix
    v_color0 = mix(u_particleStartColor, u_particleEndColor, life01);    

    // Scale
    float startScale = u_particleSizeLife.x;
    float endScale = u_particleSizeLife.y;

    float particleScale = mix(startScale, endScale, life01);

    // Make each particle mesh smaller so cubes are visible as particles.
    vec3 localPos = a_position * particleScale + instanceOffset;

    // Your GPU instanced draw currently submits identity transform,
    // but we still keep the standard model/world pipeline.
    vec4 worldPos = mul(u_model[0], vec4(localPos, 1.0));

    v_worldPos = worldPos.xyz;
    v_normal   = mul(u_model[0], vec4(a_normal.xyz, 0.0)).xyz;

    gl_Position = mul(u_modelViewProj, vec4(localPos, 1.0));
}