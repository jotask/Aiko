#version 450

layout(location = 0) in vec3 a_position;
layout(location = 2) in vec2 a_texcoord0;

layout(location = 0) out vec2 v_texcoord0;

layout(push_constant) uniform PassthroughPushConstants
{
    mat4 u_modelViewProj;
};

void main()
{
    v_texcoord0 = a_texcoord0;
    gl_Position = u_modelViewProj * vec4(a_position, 1.0);
}
