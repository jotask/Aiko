#ifndef AIKO_UI_MATERIAL_GLSL
#define AIKO_UI_MATERIAL_GLSL

layout(
    set = AIKO_GRAPHICS_MATERIAL_SET,
    binding = AIKO_MATERIAL_UBO_BINDING
) uniform MaterialUbo
{
    vec4 u_baseColor;
    vec4 u_flags;

    vec4 u_uiRect;
    vec4 u_uiStyle;
    vec4 u_uiBorderColor;
};

#endif
