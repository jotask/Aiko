#version 450
#extension GL_GOOGLE_include_directive : require

uint particleInstanceIndex()
{
    return uint(gl_InstanceIndex);
}

#include "particles_gpuinst_common.glsl"
