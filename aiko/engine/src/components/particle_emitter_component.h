#pragma once

#include "models/component.h"

namespace aiko
{

    class ParticleEmitterComponent : public Component
    {
    public:
        ParticleEmitterComponent();
        virtual ~ParticleEmitterComponent() = default;

        uint32_t getMaxParticles() const { return m_maxParticles; }
        void setMaxParticles(uint32_t value) { m_maxParticles = value; }

        float getSpawnRate() const { return m_spawnRate; }
        void setSpawnRate(float value) { m_spawnRate = value; }

        float getLifetime() const { return m_lifeTime; }
        void setLifetime(float value) { m_lifeTime = value; }

        float getStartSpeed() const { return m_startSpeed; }
        void setStartSpeed(float value) { m_startSpeed = value; }

        bool isPlaying() const { return m_playing; }
        void setPlaying(bool value) { m_playing = value; }

        void requestReset() { m_resetRequested = true; }
        bool consumeResetRequest();

    private:

        u32 m_maxParticles = 1024;
        float m_spawnRate = 0.0f;
        float m_lifeTime = 5.0f;
        float m_startSpeed = 1.0f;
        bool m_playing = true;
        bool m_resetRequested = true;
    };

}
