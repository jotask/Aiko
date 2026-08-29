#ifndef AIKO_GRAPHICS_GLSL
#define AIKO_GRAPHICS_GLSL

#extension GL_GOOGLE_include_directive : require

#include "aiko_constants.glsl"
#include "aiko_descriptor_abi.glsl"

#define MAX_LIGHTS 8

layout(
    set = AIKO_GRAPHICS_FRAME_SET,
    binding = AIKO_GRAPHICS_FRAME_BINDING
) uniform FrameUbo
{
    mat4 u_view;
    mat4 u_projection;
    mat4 u_viewProj;
    vec4 u_cameraPos;
    vec4 u_time;
    vec4 u_ambientColor;
    vec4 u_ambientIntensity;
    vec4 u_lightCount;
    vec4 u_lightType[MAX_LIGHTS];
    vec4 u_lightPosRange[MAX_LIGHTS];
    vec4 u_lightDir[MAX_LIGHTS];
    vec4 u_lightColorInt[MAX_LIGHTS];
    vec4 u_lightSpotCos[MAX_LIGHTS];
};

layout(
    set = AIKO_GRAPHICS_MATERIAL_SET,
    binding = AIKO_MATERIAL_UBO_BINDING
) uniform MaterialUbo
{
    vec4 u_baseColor;
    vec4 u_flags;
    vec4 u_particleSizeLife;
    vec4 u_particleStartColor;
    vec4 u_particleEndColor;
    vec4 u_billboardParams;
    vec4 u_nbodyRender;
};

layout(
    set = AIKO_GRAPHICS_MATERIAL_SET,
    binding = AIKO_MATERIAL_TEXTURE_BINDING
) uniform sampler2D u_texture;

layout(push_constant) uniform DrawPushConstants
{
    mat4 u_model;
    mat4 u_modelViewProj;
};

mat4 aikoModelViewProj()
{
    return u_modelViewProj;
}

mat4 aikoViewProj()
{
    return u_viewProj;
}

#endif
