#version 450

#extension GL_GOOGLE_include_directive : require

#include "aiko_graphics.glsl"

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_PointSize = AIKO_GL_POINT_SIZE;
    gl_Position = ubo.proj * ubo.view * vec4(inPosition.xyz, 1.0);
    fragColor = inColor.rgb;
}