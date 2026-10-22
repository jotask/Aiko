#version 450
#extension GL_GOOGLE_include_directive : require

#include "aiko_instance_order.glsl"

uint particleInstanceIndex()
{
    return aikoInstanceIndex();
}

#include "particles_gpuinst_common.glsl"
