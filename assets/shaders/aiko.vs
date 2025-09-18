// aiko.vs - bgfx universal vertex shader

// Attributes
attribute vec3 a_pos;
attribute vec2 a_texcoord0;
attribute vec3 a_color0;

// Varyings to fragment shader
varying vec2 v_texcoord0;
varying vec3 v_color0;

// Uniforms
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    gl_Position = u_projection * u_view * u_model * vec4(a_pos, 1.0);
    v_texcoord0 = a_texcoord0;
    v_color0 = a_color0;
}
