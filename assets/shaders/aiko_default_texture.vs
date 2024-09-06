#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
    TexCoord = aTexCoords;   
    ourColor = aColor;
    gl_Position = projection * view * model * vec4(aPos, 1.0);

}
