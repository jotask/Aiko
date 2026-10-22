#version 450
#extension GL_GOOGLE_include_directive : require

uint billboardInstanceIndex()
{
    return uint(gl_InstanceIndex);
}

#include "gpu_billboard_common.glsl"
