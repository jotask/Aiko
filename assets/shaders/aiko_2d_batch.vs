#version 330
precision highp float;

layout(location = 0) in vec2 quad_positions;
layout(location = 1) in vec4 quad_colors;
layout(location = 2) in vec2 texturePositions;

out vec4 v_color;
out vec2 v_texture;
out vec2 v_positions;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(quad_positions, 0, 1);
	v_color = quad_colors;
	v_texture = texturePositions;
	v_positions = gl_Position.xy;
}