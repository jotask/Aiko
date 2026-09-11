#version 450
#extension GL_GOOGLE_include_directive : require

#include "aiko_graphics.glsl"

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord0;
layout(location = 3) in vec4 a_color0;
layout(location = 4) in vec4 i_data0;
layout(location = 5) in vec4 i_data1;
layout(location = 6) in vec4 i_data2;
layout(location = 7) in vec4 i_data3;

layout(location = 0) out vec2 v_texcoord0;
layout(location = 1) out vec4 v_color0;
layout(location = 2) out vec3 v_normal;
layout(location = 3) out vec3 v_worldPos;

void main()
{
    vec3 position = i_data0.xyz;
    vec3 rotation = radians(i_data1.xyz);
    vec3 scale = i_data2.xyz;
    vec4 instColor = i_data3;

    float cx = cos(rotation.x);
    float sx = sin(rotation.x);
    float cy = cos(rotation.y);
    float sy = sin(rotation.y);
    float cz = cos(rotation.z);
    float sz = sin(rotation.z);

    mat3 rotationX = mat3(
        1.0, 0.0, 0.0,
        0.0, cx,  sx,
        0.0, -sx, cx
    );

    mat3 rotationY = mat3(
        cy,  0.0, -sy,
        0.0, 1.0, 0.0,
        sy,  0.0, cy
    );

    mat3 rotationZ = mat3(
        cz,  sz,  0.0,
        -sz, cz,  0.0,
        0.0, 0.0, 1.0
    );

    mat3 instanceRotation = rotationX * rotationY * rotationZ;

    vec3 localPosition = a_position * scale;
    vec3 worldPos = instanceRotation * localPosition + position;

    mat3 instanceTransform =
        instanceRotation *
        mat3(
            scale.x, 0.0, 0.0,
            0.0, scale.y, 0.0,
            0.0, 0.0, scale.z
        );

    v_texcoord0 = a_texcoord0;
    v_color0 = instColor;
    v_worldPos = worldPos;
    v_normal =
        normalize(
            transpose(inverse(instanceTransform)) *
            a_normal
        );

    gl_Position =
        aikoViewProj() *
        vec4(worldPos, 1.0);
}
