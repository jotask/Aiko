#include "bgfx_compute.sh"

BUFFER_RW(u_pos, vec4, 0);
BUFFER_RW(u_vel, vec4, 1);
BUFFER_RW(u_life, vec4, 2);

uniform vec4 u_params;     // x=count, y=lifetime, z=startSpeed
uniform vec4 u_emitterPos; // xyz = emitter world position

NUM_THREADS(64, 1, 1)
void main()
{
    uint idx = gl_GlobalInvocationID.x;
    if (idx >= uint(u_params.x))
        return;

    // Start all particles dead and inactive.
    u_pos[idx] = vec4(u_emitterPos.xyz, 1.0);
    u_vel[idx] = vec4(0.0, 0.0, 0.0, 0.0);
    u_life[idx] = vec4(0.0, u_params.y, 0.0, 0.0);
}