#ifndef AIKO_INSTANCE_ORDER_GLSL
#define AIKO_INSTANCE_ORDER_GLSL

layout(std430, set = 2, binding = 15) readonly buffer AikoInstanceOrder
{
    uvec2 aikoSortEntries[];
};

uint aikoInstanceIndex()
{
    return aikoSortEntries[uint(gl_InstanceIndex)].y;
}

#endif