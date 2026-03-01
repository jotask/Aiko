#include "bgfx_compute.sh"
IMAGE2D_WO(u_dst, rgba32f, 1);
uniform vec4 u_params;

NUM_THREADS(64, 1, 1)
void main()
{
    uint i = gl_GlobalInvocationID.x;
    uint count = uint(u_params.x);
    if (i >= count) return;

    imageStore(u_dst, ivec2(int(i), 0), vec4(1.0, 2.0, 3.0, 4.0));
}