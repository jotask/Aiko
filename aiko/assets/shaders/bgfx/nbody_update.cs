#include "bgfx_compute.sh"

// slot 0 = current position/mass
BUFFER_RO(u_posMass, vec4, 0);

// slot 1 = current velocity
BUFFER_RO(u_vel, vec4, 1);

// slot 2 = next position/mass
BUFFER_WO(u_posMassNext, vec4, 2);

// slot 3 = next velocity
BUFFER_WO(u_velNext, vec4, 3);

uniform vec4 u_params;   // x = dt, y = timeScale, z = softening, w = count
uniform vec4 u_gravity;  // xyz = gravitational constant, w = central mass

NUM_THREADS(64, 1, 1)
void main()
{
    uint idx = gl_GlobalInvocationID.x;
    uint count = uint(u_params.w);

    if (idx >= count)
        return;

    float dt        = u_params.x;
    float timeScale = u_params.y;
    float softening = u_params.z;

    vec4 selfPosMass = u_posMass[idx];
    vec3 pos = selfPosMass.xyz;
    float selfMass = selfPosMass.w;

    vec3 vel = u_vel[idx].xyz;

    vec3 accel = vec3(0.0);

    // Use magnitude of u_gravity.xyz as scalar G for now
    float G = length(u_gravity.xyz);

    // Naive O(n^2) all-pairs accumulation
    for (uint j = 0; j < count; ++j)
    {
        if (j == idx)
            continue;

        vec4 otherPosMass = u_posMass[j];
        vec3 delta = otherPosMass.xyz - pos;

        float dist2 = dot(delta, delta) + softening * softening;
        float invDist = inversesqrt(dist2);
        float invDist3 = invDist * invDist * invDist;

        accel += delta * (G * otherPosMass.w * invDist3);
    }

    // Optional central attractor at origin.
    // u_gravity.w = central mass
    float centralMass = u_gravity.w;

    if (centralMass > 0.0)
    {
        vec3 deltaCenter = -pos;
        float dist2Center = dot(deltaCenter, deltaCenter) + softening * softening;
        float invDistCenter = inversesqrt(dist2Center);
        float invDistCenter3 = invDistCenter * invDistCenter * invDistCenter;

        accel += deltaCenter * (G * centralMass * invDistCenter3);
    }

    float step = dt * timeScale;

    vel += accel * step;
    pos += vel * step;

    u_posMassNext[idx] = vec4(pos, selfMass);
    u_velNext[idx] = vec4(vel, 0.0);
}