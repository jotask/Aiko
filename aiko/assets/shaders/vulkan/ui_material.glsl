#ifndef AIKO_UI_MATERIAL_GLSL
#define AIKO_UI_MATERIAL_GLSL

layout(
    set = AIKO_GRAPHICS_MATERIAL_SET,
    binding = AIKO_MATERIAL_UBO_BINDING
) uniform MaterialUbo
{
    vec4 u_baseColor;
    vec4 u_flags;

    // One UI draw can carry up to 128 differently styled primitives.
    // Keep in sync with UIRenderer::render(). Each array uses std140 stride.
    vec3 u_uiRectPosRadius[128];
    vec3 u_uiRectSizeBorder[128];
    vec3 u_uiBorderRgb[128];
    float u_uiBorderAlpha[128];
};

#endif
