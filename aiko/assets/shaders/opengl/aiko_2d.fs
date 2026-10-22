#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform vec4 u_color;
uniform sampler2D u_texture;

void main()
{
	vec4 final = texture(u_texture, TexCoord) * u_color;
	if(final.a < 0.001)
	{
		discard;
	}
	FragColor = final;
}