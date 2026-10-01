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

struct SpringLookupEntry
{
    uint pairKey;
    uint springIndex;
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

uint hashSpringPair(uint pairKey)
{
    pairKey ^= pairKey >> 16u;
    pairKey *= 0x7FEB352Du;
    pairKey ^= pairKey >> 15u;
    pairKey *= 0x846CA68Bu;
    pairKey ^= pairKey >> 16u;

    return pairKey;
}

#endif