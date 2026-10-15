#version 450
#extension GL_GOOGLE_include_directive : require

#define AIKO_BILLBOARD_MATERIAL_COLOR
#include "aiko_instance_order.glsl"

uint billboardInstanceIndex()
{
    return aikoInstanceIndex();
}

#include "gpu_billboard_common.glsl"
