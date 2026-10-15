#include "aiko_graphics.glsl"

layout(location = 0) in vec3 a_position;
layout(location = 2) in vec2 a_texcoord0;

layout(std430, set = 2, binding = 7) readonly buffer BillboardPositions
{
    vec4 positions[];
};

layout(std430, set = 2, binding = 8) readonly buffer BillboardColors
{
    vec4 colors[];
};

layout(location = 0) out vec2 v_texcoord0;
layout(location = 1) out vec4 v_color0;
layout(location = 2) out vec3 v_normal;
layout(location = 3) out vec3 v_worldPos;

void main()
{
    uint index = billboardInstanceIndex();
    vec4 instance = positions[index];

    vec3 cameraRight = vec3(
        u_view[0][0],
        u_view[1][0],
        u_view[2][0]);

    vec3 cameraUp = vec3(
        u_view[0][1],
        u_view[1][1],
        u_view[2][1]);

    vec3 worldPosition = instance.xyz + cameraRight * a_position.x * instance.w + cameraUp * a_position.y * instance.w;

    v_texcoord0 = a_texcoord0;
    v_color0 = colors[index];
    v_normal = normalize(cross(cameraRight, cameraUp));
    v_worldPos = worldPosition;

    gl_Position = aikoModelViewProj() * vec4(worldPosition, 1.0);
}
