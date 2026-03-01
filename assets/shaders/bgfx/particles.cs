#include "bgfx_compute.sh"

// RW buffer at stage 0 (vec4 per pixel)
BUFFER_RW(u_data, vec4, 0);

// Output image at stage 1
IMAGE2D_WO(u_output, rgba8, 1);

// x = time, y = width, z = height, w = unused
uniform vec4 u_params;

NUM_THREADS(8, 8, 1)
void main()
{
    uvec3 gid = gl_GlobalInvocationID;

    ivec2 size = imageSize(u_output);
    if (gid.x >= uint(size.x) || gid.y >= uint(size.y))
        return;

    uint idx = gid.y * uint(size.x) + gid.x;

    vec4 v = u_data[idx];

    // Simple animation
    v.x += 0.01;
    if (v.x > 1.0) v.x = 0.0;

    u_data[idx] = v;

    // Visualize buffer value
    float fx = float(gid.x) / float(max(size.x - 1, 1));
    float fy = float(gid.y) / float(max(size.y - 1, 1));

    vec4 color = vec4(v.x, fy, fx, 1.0);
    imageStore(u_output, ivec2(gid.xy), color);
}