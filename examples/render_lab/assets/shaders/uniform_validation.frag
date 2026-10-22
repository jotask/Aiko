#version 450

layout(location = 0) in vec2 v_uv;
layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform MaterialData
{
    bool u_labBool;
    int u_labInt;
    uint u_labUInt;
    float u_labFloat;

    vec2 u_labVec2;
    vec3 u_labVec3;
    vec4 u_labVec4;

    mat4 u_labMat4;
};

void main()
{
    float boolValue = u_labBool ? 1.0 : 0.0;
    float intValue = float(u_labInt + 42);
    float uintValue = float(u_labUInt) / 42.0;

    vec4 matrixValue = u_labMat4 * vec4(1.0);

    vec3 color =
        vec3(
            boolValue * u_labVec3.r,
            uintValue * u_labVec3.g,
            u_labFloat
        );

    color += vec3(u_labVec2.x * 0.1);
    color += u_labVec4.rgb * 0.1;
    color += matrixValue.rgb * 0.05;
    color += vec3(intValue * 0.01);

    outColor = vec4(color, 1.0);
}
