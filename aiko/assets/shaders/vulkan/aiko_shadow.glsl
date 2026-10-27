#ifndef AIKO_SHADOW_GLSL
#define AIKO_SHADOW_GLSL

#include "aiko_descriptor_abi.glsl"

layout(
    set = AIKO_GRAPHICS_SHADOW_SET,
    binding = AIKO_SHADOW_UBO_BINDING
) uniform ShadowUbo
{
    mat4 u_lightViewProjection;
    vec4 u_shadowParams;
};

layout(
    set = AIKO_GRAPHICS_SHADOW_SET,
    binding = AIKO_SHADOW_TEXTURE_BINDING
) uniform sampler2D u_shadowMap;

#endif
