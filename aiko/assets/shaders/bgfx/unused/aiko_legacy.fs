// aiko.fs - bgfx universal fragment shader

// Varyings coming from vertex shader
varying vec2 v_texcoord0;
varying vec3 v_color0;

// Texture samplers
uniform sampler2D s_texture1;
uniform sampler2D s_texture2;

void main()
{
    // Linearly interpolate between textures (optional)
    // vec4 texColor = mix(texture2D(s_texture1, v_texcoord0), texture2D(s_texture2, v_texcoord0), 0.2);
    
    // For now, just output vertex color
    gl_FragColor = vec4(v_color0, 1.0);
}
