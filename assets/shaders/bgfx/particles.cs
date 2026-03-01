#include "bgfx_compute.sh"

// RW buffer (stage 0)
BUFFER_RW(u_data, vec4, 0);

// Output texture (stage 1)
IMAGE2D_WO(u_output, rgba8, 1);

NUM_THREADS(8,8,1)
void main()
{
    uvec3 gid = gl_GlobalInvocationID;

    ivec2 size = imageSize(u_output);

    if (gid.x >= uint(size.x) || gid.y >= uint(size.y))
        return;

    uint index = gid.y * uint(size.x) + gid.x;

    vec4 v = u_data[index];

    // animate value every frame
    v.x += 0.01;
    if (v.x > 1.0)
        v.x = 0.0;

    u_data[index] = v;

    // visualize buffer value
    imageStore(u_output, ivec2(gid.xy),
               vec4(v.x, 0.2, 0.8, 1.0));
}