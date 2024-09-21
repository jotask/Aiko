#version 330
precision highp float;

in vec4 v_color;
in vec2 v_texture;

out vec4 color;

uniform sampler2D u_sampler;

void main()
{
    vec4 texColor = texture(u_sampler, v_texture);
    vec4 finalColor = v_color * texColor;
    if (finalColor.a < 0.1) // Adjust threshold for transparency
    {
		discard;
    }
    color = finalColor;
}