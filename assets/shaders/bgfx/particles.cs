#include "bgfx_compute.sh"

// position buffer
BUFFER_RW(u_pos, vec4, 0);

// velocity buffer
BUFFER_RW(u_vel, vec4, 1);

// debug output
IMAGE2D_WO(u_output, rgba8, 2);

// x = dt
// y = particle count
uniform vec4 u_params;

NUM_THREADS(64,1,1)
void main()
{
    uint id = gl_GlobalInvocationID.x;
    uint count = uint(u_params.y);

    if (id >= count)
        return;

    float dt = u_params.x;

    vec3 p = u_pos[id].xyz;
    vec3 v = u_vel[id].xyz;

    vec3 center = vec3(0.0);
    vec3 r = p - center;

    float dist = max(length(r), 0.001);
    vec3 dir = r / dist;

    // centripetal acceleration toward center
    float G = 2.0;              // strength
    vec3 acc = -dir * (G / (dist*dist));   // inverse-square

    // optional damping to keep stable
    float damping = 0.05;
    v *= exp(-damping * dt);

    v += acc * dt;
    p += v * dt;

    u_pos[id] = vec4(p, 1.0);
    u_vel[id] = vec4(v, 0.0);

    // DEBUG VISUALIZATION
    #if 0
    if ((id & 255u) == 0u)
    {
        ivec2 size = imageSize(u_output);

        vec2 uv = clamp(p.xy * 0.5 + 0.5, 0.0, 1.0);
        ivec2 pixel = ivec2(uv * vec2(size-1));

        pixel = clamp(pixel, ivec2(0), size - ivec2(1));

        imageStore(u_output, pixel, vec4(1,1,1,1));
    }
    #endif
}