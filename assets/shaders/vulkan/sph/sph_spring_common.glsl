#ifndef SPH_SPRING_COMMON_GLSL
#define SPH_SPRING_COMMON_GLSL

const uint EMPTY_SPRING_KEY = 0xFFFFFFFFu;

struct Spring
{
    uint pairKey;
    float restLength;
    uint enabled;
    uint padding;
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

uint hashPairKey(uint key)
{
    key ^= key >> 16;
    key *= 0x7feb352du;
    key ^= key >> 15;
    key *= 0x846ca68bu;
    key ^= key >> 16;

    return key;
}

#endif