#pragma once

#include "models/component.h"

namespace aiko
{

    class NBodyComponent : public Component
    {
    public:

        enum class NBodyInitMode
        {
            Sphere = 0,
            DiskOrbit = 1
        };

        NBodyComponent();
        virtual ~NBodyComponent() = default;

        uint64_t getMaxBodies() const { return m_maxBodies; }
        void setMaxBodies(uint64_t value) { m_maxBodies = value; }

        vec3 getGravitationalConstant() const { return m_gravitationalConstant; }
        void setGravitationalConstant(vec3 value) { m_gravitationalConstant = value; }

        NBodyInitMode getInitMode() const { return m_initMode; }
        void setInitMode(NBodyInitMode value) { m_initMode = value; }

        float getCentralMass() const { return m_centralMass; }
        void setCentralMass(float value) { m_centralMass = value; }

        float getSoftening() const { return m_softening; }
        void setSoftening(float value) { m_softening = value; }

        float getTimeScale() const { return m_timeScale; }
        void setTimeScale(float value) { m_timeScale = value; }

        float getInitialRadius() const { return m_initialRadius; }
        void setInitialRadius(float value) { m_initialRadius = value; }

        float getInitialSpeed() const { return m_initialSpeed; }
        void setInitialSpeed(float value) { m_initialSpeed = value; }

        float getRenderScale() const { return m_renderScale; }
        void setRenderScale(float value) { m_renderScale = value; }

        bool isPlaying() const { return m_playing; }
        void setPlaying(bool value) { m_playing = value; }

        void requestReset() { m_resetRequested = true; }
        bool consumeResetRequest();

        // Presets
        void applyStablePreset();
        void applyChaoticPreset();
        void applyStressTestPreset();

    private:

        uint64_t m_maxBodies;
        NBodyInitMode m_initMode;
        vec3 m_gravitationalConstant;
        float m_centralMass;
        float m_softening;
        float m_timeScale;
        float m_initialRadius;
        float m_initialSpeed;
        float m_renderScale;
        bool m_playing;
        bool m_resetRequested;

    };

}
