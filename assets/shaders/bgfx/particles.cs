#include "bgfx_compute.sh"

struct Particle
{
    vec4 position;
    vec4 velocity;
};

BUFFER_RW(particles, Particle, 0);

NUM_THREADS(64,1,1)
void main()
{
    uint id = gl_GlobalInvocationID.x;

    Particle p = particles[id];

    p.position.xyz += p.velocity.xyz * 0.016;

    particles[id] = p;
}