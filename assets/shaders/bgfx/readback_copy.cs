#include "bgfx_compute.sh"

BUFFER_RO(u_src, vec4, 0);
IMAGE2D_WO(u_dst, rgba32f, 1);
uniform vec4 u_params; // x = vec4Count

NUM_THREADS(64, 1, 1)
void main()
{
    uint idx = gl_GlobalInvocationID.x;
    if (idx >= uint(u_params.x))
        return;

    vec4 v = u_src[idx];
    imageStore(u_dst, ivec2(int(idx), 0), v);
}