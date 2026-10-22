#version 450
#extension GL_GOOGLE_include_directive : require

#include "aiko_graphics.glsl"
#include "aiko_color.glsl"

layout(location = 0) in vec2 v_uv;
layout(location = 0) out vec4 outColor;

void main()
{
    // Material colors are stored as sRGB values.
    vec3 uniformLinear = aikoSrgbToLinear(u_baseColor.rgb);

    // The sRGB texture format performs decoding automatically.
    vec3 textureLinear = texture(u_texture, v_uv).rgb;

    const float expected = 0.215861;
    const float tolerance = 0.005;

    bool uniformOK =
        all(lessThan(abs(uniformLinear - vec3(expected)),
                     vec3(tolerance)));

    bool textureOK =
        all(lessThan(abs(textureLinear - vec3(expected)),
                     vec3(tolerance)));

    bool match =
        all(lessThan(abs(uniformLinear - textureLinear),
                     vec3(tolerance)));

    if (v_uv.x < 0.3333)
    {
        outColor = vec4(uniformLinear, 1.0);
    }
    else if (v_uv.x < 0.6667)
    {
        outColor = vec4(textureLinear, 1.0);
    }
    else
    {
        outColor = vec4(
            uniformOK && textureOK && match
                ? vec3(0.0, 1.0, 0.0)
                : vec3(1.0, 0.0, 0.0),
            1.0
        );
    }
}
