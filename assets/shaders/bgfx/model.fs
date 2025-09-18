$input v_texcoord0, v_color0, v_normal

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

    // enable lighting
    if (u_flags.z > 0.5)
    {
        vec4 u_ambient = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        vec4 u_lightDir = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        vec4 u_lightColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);

        // normalize light and normal
        vec3 N = normalize(v_normal.xyz);
        vec3 L = normalize(u_lightDir.xyz);

        // Lambert diffuse
        float NdotL = max(dot(N, L), 0.0);

        // Convert base color to vec3 (RGB)
        vec3 b = base;

        // Ambient + diffuse
        vec3 ambient = b * u_ambient.x;
        vec3 diffuse = b * NdotL * u_lightColor.rgb;

        vec3 finalColor = ambient + diffuse;

        base *=  vec4(finalColor, 1.0f);
    }

    gl_FragColor = base;
}
