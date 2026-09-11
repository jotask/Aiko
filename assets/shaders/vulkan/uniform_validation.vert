#version 450

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in vec4 a_color;

layout(set = 0, binding = 0) uniform FrameData
{
    mat4 u_view;
    mat4 u_projection;
    mat4 u_viewProj;
    vec4 u_cameraPos;
    vec4 u_time;
    vec4 u_ambientColor;
    vec4 u_ambientIntensity;
    vec4 u_lightCount;
};

layout(push_constant) uniform PushConstants
{
    mat4 u_model;
    mat4 u_modelViewProj;
};

layout(location = 0) out vec2 v_uv;

void main()
{
    v_uv = a_texCoord;
    gl_Position = u_modelViewProj * vec4(a_position, 1.0);
}
