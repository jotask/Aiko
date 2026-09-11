#version 450

layout(set = 0, binding = 0) uniform sampler2D u_scene;

layout(location = 0) in vec2 v_texcoord0;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = texture(u_scene, v_texcoord0);
}
