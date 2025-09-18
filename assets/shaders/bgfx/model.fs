$input v_texcoord0, v_color0

#include "bgfx_shader.sh"

uniform vec4 u_baseColor;     // fallback color (rgba)
uniform vec4 u_flags;         // x = useTexture, y = useVertexColor

SAMPLER2D(u_texture, 0);      // diffuse texture slot

void main()
{
    vec4 base = u_baseColor;

    // Use texture if enabled
    if (u_flags.x > 0.5)
    {
        base = texture2D(u_texture, v_texcoord0);
    }

    // Multiply by vertex color if enabled
    if (u_flags.y > 0.5)
    {
        base *= v_color0;
    }

    gl_FragColor = base;
}
