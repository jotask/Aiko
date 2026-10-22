$input a_position, a_texcoord0, a_color0, a_normal
$output v_texcoord0, v_color0, v_normal, v_worldPos

#include "bgfx_shader.sh"
#include "bgfx_compute.sh"

// slot 7 = world positions
BUFFER_RO(u_pos, vec4, 7);

// x = billboard size
uniform vec4 u_billboardParams;

void main()
{
    uint iid = uint(gl_InstanceID);

    vec3 center = u_pos[iid].xyz;
    float size = u_billboardParams.x;

    // Use view-matrix rows/columns carefully so the billboard faces the camera.
    // These two vectors are the camera-space world right/up axes.
    vec3 cameraRight = vec3(u_view[0].x, u_view[0].y, u_view[0].z);
    vec3 cameraUp    = vec3(u_view[1].x, u_view[1].y, u_view[1].z);

    vec2 quad = a_position.xy;

    vec3 worldPos = center
        + cameraRight * (quad.x * size)
        + cameraUp    * (quad.y * size);

    v_texcoord0 = a_texcoord0;
    v_color0    = a_color0;
    v_worldPos  = worldPos;
    v_normal    = vec3(0.0, 0.0, 1.0);

    gl_Position = mul(u_viewProj, vec4(worldPos, 1.0));
}