#include "bgfx_compute.sh"

BUFFER_RW(u_pos, vec4, 0);
BUFFER_RW(u_vel, vec4, 1);
BUFFER_RW(u_life, vec4, 2);

uniform vec4 u_params; // x=dt, y=lifetime

NUM_THREADS(64, 1, 1)
void main()
{
    uint idx = gl_GlobalInvocationID.x;
    float dt = u_params.x;

    vec4 pos = u_pos[idx];
    vec4 vel = u_vel[idx];
    vec4 life = u_life[idx];

    if (life.x > 0.0)
    {
        pos.xyz += vel.xyz * dt;
        life.x -= dt;
    }
    else
    {
        // very simple respawn at origin
        pos.xyz = vec3(0.0, 0.0, 0.0);
        life.x = life.y;
    }

    u_pos[idx] = pos;
    u_life[idx] = life;
}