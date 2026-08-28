#ifndef AIKO_GRAPHICS_GLSL
#define AIKO_GRAPHICS_GLSL

#define MAX_LIGHTS 8

layout(set = 0, binding = 0) uniform FrameUbo
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

layout(set = 1, binding = 0) uniform MaterialUbo
{
    vec4 u_baseColor;
    vec4 u_flags;
    vec4 u_particleSizeLife;
    vec4 u_particleStartColor;
    vec4 u_particleEndColor;
    vec4 u_billboardParams;
    vec4 u_nbodyRender;
};

layout(set = 1, binding = 1) uniform sampler2D u_texture;

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
