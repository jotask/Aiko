#ifndef SPH_SPRING_COMMON_GLSL
#define SPH_SPRING_COMMON_GLSL

const uint INVALID_SPRING_INDEX = 0xFFFFFFFFu;

struct Spring
{
    uint pairKey;
    float restLength;
    uint nextA;
    uint nextB;
};

uint makePairKey(uint particleA, uint particleB)
{
    uint a = min(particleA, particleB);
    uint b = max(particleA, particleB);
    return (a << 16) | b;
}

uint getParticleA(uint pairKey)
{
    return pairKey >> 16;
}

uint getParticleB(uint pairKey)
{
    return pairKey & 0xFFFFu;
}

#endif