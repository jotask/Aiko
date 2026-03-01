// gradient.cs (compute shader)
// BGFX nbody-style compute shader format.

#include "bgfx_compute.sh"

// Output image bound at stage/register 0 via bgfx::setImage(0, ...).
// Use rgba8 for the first test.
IMAGE2D_WO(u_output, rgba8, 0);

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

    vec4 color = vec4(fx, fy, 0.35, 1.0);
    imageStore(u_output, ivec2(gid.xy), color);
}