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

    vec3 p = u_seedPos[id].xyz;
    float dist = max(length(p), 0.001);
    vec3 dir = p / dist;
    vec3 tang = normalize(vec3(-dir.y, dir.x, 0.0));
    float speed = sqrt(2.0 / dist); // uses same G=2.0 as sim
    u_vel[id] = vec4(tang * speed, 0.0);
    u_pos[id] = vec4(p, 1.0);
    
}