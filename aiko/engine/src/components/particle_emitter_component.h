#pragma once

#include "models/component.h"

namespace aiko
{

    class ParticleEmitterComponent : public Component
    {
    public:

        enum class ParticleSpawnShape
        {
            Point = 0,
            Circle = 1,
            Box = 2,
            Sphere = 3,
        };

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

        ParticleSpawnShape getSpawnShape() const { return m_spawnShape; }
        void setSpawnShape(ParticleSpawnShape value) { m_spawnShape = value; }

        float getSpawnRadius() const { return m_spawnRadius; }
        void setSpawnRadius(float value) { m_spawnRadius = value; }

        vec3 getSpawnBoxExtents() const { return m_spawnBoxExtents; }
        void setSpawnBoxExtents(vec3 value) { m_spawnBoxExtents = value; }

        vec3 getDirection() const { return m_direction; }
        void setDirection(vec3 value) { m_direction = value; }

        float getDirectionRandomness() const { return directionRandomness; }
        void setDirectionRandomness(float value) { directionRandomness = value; }

        vec3 getGravity() const { return m_gravity; }
        void setGravity(vec3 value) { m_gravity = value; }

    private:

        // Spawn
        ParticleSpawnShape m_spawnShape;
        float m_spawnRadius;
        vec3 m_spawnBoxExtents;

        vec3 m_direction = {0.0f, 0.0f, 0.0f};
        float directionRandomness = 0.0f;
        vec3 m_gravity = {0.0f};

        // Config
        u32 m_maxParticles = 1024;
        float m_spawnRate = 0.0f;
        float m_lifeTime = 5.0f;
        float m_startSpeed = 1.0f;
        bool m_playing = true;
        bool m_resetRequested = true;
    };

}
