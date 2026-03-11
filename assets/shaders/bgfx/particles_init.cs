#include "bgfx_compute.sh"

BUFFER_RW(u_pos, vec4, 0);
BUFFER_RW(u_vel, vec4, 1);
BUFFER_RW(u_life, vec4, 2);

uniform vec4 u_params; // x=count, y=lifetime, z=startSpeed

NUM_THREADS(64, 1, 1)
void main()
{
    uint idx = gl_GlobalInvocationID.x;
    if (idx >= uint(u_params.x))
        return;

    float t = float(idx) / max(u_params.x - 1.0, 1.0);
    float a = t * 6.2831853;
    float r = 0.25;

    vec3 pos = vec3(cos(a) * r, sin(a) * r, 0.0);
    vec3 vel = vec3(-sin(a), cos(a), 0.0) * u_params.z;

    u_pos[idx] = vec4(pos, 1.0);
    u_vel[idx] = vec4(vel, 0.0);
    u_life[idx] = vec4(u_params.y, u_params.y, 0.0, 0.0); // x = remaining, y = total
}