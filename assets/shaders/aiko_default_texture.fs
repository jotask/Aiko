#version 330 core

in vec3 ourColor;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D tex;

void main()
{

	// linearly interpolate between both textures (80% container, 20% awesomeface)
	FragColor = texture(tex, TexCoord);

}
