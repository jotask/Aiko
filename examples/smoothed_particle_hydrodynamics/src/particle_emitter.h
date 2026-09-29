#pragma once

#include "sph_types.h"

namespace sph
{

    struct EmitterSettings
    {
        aiko::vec3 position = {};
        aiko::vec3 direction = {};
        float size = 1.0f;
        float spawnInterval = 0.25f;
        size_t amount = 10;
        float velocity = 1.0f;
        float angularVelocity = 1.0f;
    };

    class ParticleEmitter
    {
    public:
        void init(const EmitterSettings settings);

        void spawn(float dt, aiko::vector<SPHParticle>& spawnedParticles);
        void rotate(float dt);
        void move(float dt);

        const EmitterSettings& settings() const { return m_settings; }

    private:
        EmitterSettings m_settings = {};
        float m_time = 0.0f;
    };

}