// gradient.cs (compute shader)
// BGFX nbody-style compute shader format.

#include "bgfx_compute.sh"

// Output image bound at stage/register 0 via bgfx::setImage(0, ...).
// Use rgba8 for the first test.
IMAGE2D_WO(u_output, rgba8, 0);
uniform vec4 u_params; // x = time

NUM_THREADS(8, 8, 1)
void main()
{
    // Global invocation id (pixel coordinate)
    uvec3 gid = gl_GlobalInvocationID;

    ivec2 size = imageSize(u_output);

    if (gid.x >= uint(size.x) || gid.y >= uint(size.y))
        return;

    float fx = float(gid.x) / float(max(size.x - 1, 1));
    float fy = float(gid.y) / float(max(size.y - 1, 1));
    float t  = u_params.x;

    float r = 0.65 + 0.20 * sin(fx * 10.0 + t) + 0.15 * sin(fy * 6.0 + t * 0.7);
    float g = 0.65 + 0.20 * sin(fy * 12.0 + t * 1.3) + 0.15 * sin((fx + fy) * 5.0 + t * 0.4);
    float b = 0.65 + 0.20 * sin((fx + fy) * 8.0 + t * 0.9) + 0.15 * sin(fx * 4.0 - t * 0.6);

    r = clamp(r, 0.0, 1.0);
    g = clamp(g, 0.0, 1.0);
    b = clamp(b, 0.0, 1.0);

    imageStore(u_output, ivec2(gid.xy), vec4(r, g, b, 1.0));
}