#include "bgfx_compute.sh"

BUFFER_RW(u_output, vec4, 0);
uniform vec4 u_params; // x = count

NUM_THREADS(64, 1, 1)
void main()
{
    uint idx = gl_GlobalInvocationID.x;
    if (idx >= uint(u_params.x))
    {
        return;
    }

    u_output[idx] = vec4(float(idx), 42.0, 0.0, 1.0);
}