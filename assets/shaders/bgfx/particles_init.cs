#include "bgfx_compute.sh"

BUFFER_RO(u_seedPos, vec4, 0);
BUFFER_RO(u_seedVel, vec4, 1);

BUFFER_RW(u_pos, vec4, 2);
BUFFER_RW(u_vel, vec4, 3);

uniform vec4 u_params; // y = count

NUM_THREADS(64,1,1)
void main()
{
    uint id = gl_GlobalInvocationID.x;
    uint count = uint(u_params.y);

    if (id >= count)
        return;

    u_pos[id] = u_seedPos[id];
    u_vel[id] = u_seedVel[id];
}