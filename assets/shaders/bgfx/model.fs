$input v_texcoord0, v_color0, v_normal

#include "bgfx_shader.sh"

uniform vec4  u_baseColor;   // rgba
uniform vec4  u_flags;       // x=useTexture, y=useVertexColor, z=lit
uniform vec4  u_lightDir;    // xyz = direction (world or view space, consistent with v_normal)
uniform vec4  u_lightColor;  // rgb
uniform float u_ambient;     // scalar

SAMPLER2D(u_texture, 0);

void main()
{
    vec4 base = u_baseColor;

    // Texture (tint, don't replace)
    if (u_flags.x > 0.5)
    {
        vec4 tex = texture2D(u_texture, v_texcoord0);
        base *= tex;
    }

    // Vertex color
    if (u_flags.y > 0.5)
    {
        base *= v_color0;
    }

    // Lighting (Lambert)
    if (u_flags.z > 0.5)
    {
        vec3 N = normalize(v_normal.xyz);
        vec3 L = normalize(u_lightDir.xyz);

        float NdotL = max(dot(N, L), 0.0);

        vec3 albedo = base.rgb;
        vec3 ambient = albedo * u_ambient;
        vec3 diffuse = albedo * NdotL * u_lightColor.rgb;

        base.rgb = ambient + diffuse;
    }

    gl_FragColor = base;
}