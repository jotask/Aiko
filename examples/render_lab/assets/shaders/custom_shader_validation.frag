#version 450

layout(location = 0) in vec3 v_localPosition;
layout(location = 0) out vec4 outColor;

void main()
{
    vec3 p = abs(v_localPosition);

    outColor = vec4(
        0.15 + p.x * 0.85,
        0.05 + p.y * 0.35,
        0.20 + p.z * 0.80,
        1.0
    );
}
