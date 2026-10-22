#include "bgfx_compute.sh"

BUFFER_RW(u_posMass, vec4, 0);
BUFFER_RW(u_vel, vec4, 1);

uniform vec4 u_params;   // x = count, y = initialRadius, z = initialSpeed, w = softening
uniform vec4 u_origin;   // xyz = simulation origin
uniform vec4 u_initMode; // x = init mode
uniform vec4 u_gravity;  // xyz = gravity / directional bias

NUM_THREADS(64, 1, 1)
void main()
{
    uint idx = gl_GlobalInvocationID.x;
    uint count = uint(u_params.x);

    if (idx >= count)
        return;

    float initialRadius = u_params.y;
    float initialSpeed  = u_params.z;

    uint mode = uint(u_initMode.x);

    // Deterministic pseudo-random values from index
    float fi = float(idx);

    float a = fract(sin(fi * 12.9898) * 43758.5453);
    float b = fract(sin((fi + 17.0) * 78.233) * 12345.6789);
    float c = fract(sin((fi + 43.0) * 39.425) * 24680.1357);

    vec3 pos;
    vec3 vel;

    // Mode 1 = flat disk with orbit-like tangential velocity
    if (mode == 1u)
    {
        float angle = a * 6.28318530718;

        // Keep some minimum radius so bodies are not all near the center.
        float r = max(0.25, b) * initialRadius;

        pos = u_origin.xyz + vec3(cos(angle) * r, 0.0, sin(angle) * r);

        vec3 radial = normalize(pos - u_origin.xyz + vec3(1e-5));
        vec3 tangent = vec3(-radial.z, 0.0, radial.x);

        // Simple orbit-like speed curve.
        float speed = initialSpeed / sqrt(max(r, 0.25));
        vel = tangent * speed;
    }
    else
    {
        // Mode 0 = random sphere
        vec3 dir = normalize(vec3(a * 2.0 - 1.0, b * 2.0 - 1.0, c * 2.0 - 1.0));
        float radius = initialRadius * pow(fract(sin((fi + 91.0) * 15.123) * 13579.2468), 1.0 / 3.0);

        pos = u_origin.xyz + dir * radius;

        // Simple tangential-ish initial velocity around origin
        vec3 radial = normalize(pos - u_origin.xyz + vec3(1e-5));
        vec3 up = abs(radial.y) > 0.99 ? vec3(1.0, 0.0, 0.0) : vec3(0.0, 1.0, 0.0);
        vec3 tangent = normalize(cross(up, radial));

        vel = tangent * initialSpeed;
    }

    // For now, uniform mass = 1
    float mass = 1.0;

    u_posMass[idx] = vec4(pos, mass);
    u_vel[idx] = vec4(vel, 0.0);
}