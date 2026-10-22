#version 450
#extension GL_GOOGLE_include_directive : require

#include "aiko_graphics.glsl"
#include "aiko_color.glsl"

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord0;
layout(location = 3) in vec4 a_color0;
layout(location = 8) in vec4 a_tangent;

layout(location = 0) out vec2 v_texcoord0;
layout(location = 1) out vec4 v_color0;
layout(location = 2) out vec3 v_normal;
layout(location = 3) out vec3 v_worldPos;
layout(location = 4) out vec4 v_tangent;

void main()
{
    v_texcoord0 = a_texcoord0;
    v_color0 = vec4(aikoSrgbToLinear(a_color0.rgb), a_color0.a);

    vec4 wpos = u_model * vec4(a_position, 1.0);
    v_worldPos = wpos.xyz;
    v_normal = normalize(transpose(inverse(mat3(u_model))) * a_normal);

    mat3 model3x3 = mat3(u_model);

    float orientationSign = determinant(model3x3) < 0.0 ? -1.0 : 1.0;

    v_tangent = vec4(model3x3 * a_tangent.xyz, a_tangent.w * orientationSign
   );

    gl_PointSize = u_flags.w;

    gl_Position = aikoModelViewProj() * vec4(a_position, 1.0);
}
