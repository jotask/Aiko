#include "nbody_component.h"

namespace nbody
{
    
    NBodyComponent::NBodyComponent()
        : Component("NBodyComponent")
        , m_maxBodies(1024)
        , m_initMode(NBodyInitMode::Sphere)
        , m_gravitationalConstant(aiko::vec3(1.0f, 1.0f, 1.0f))
        , m_centralMass(0.0f)
        , m_softening(0.1f)
        , m_timeScale(1.0f)
        , m_initialRadius(5.0f)
        , m_initialSpeed(0.5f)
        , m_renderScale(0.1f)
        , m_playing(true)
        , m_resetRequested(false)
    {
    
    }

    bool NBodyComponent::consumeResetRequest()
    {
        const bool value = m_resetRequested;
        m_resetRequested = false;
        return value;
    }

    void NBodyComponent::applyStablePreset()
    {
        setMaxBodies(1024);
        setInitMode(NBodyInitMode::DiskOrbit);
        setCentralMass(500.0f);
        setGravitationalConstant(aiko::vec3(0.4f));
        setSoftening(0.2f);
        setTimeScale(1.0f);
        setInitialRadius(6.0f);
        setInitialSpeed(1.0f);
        setRenderScale(0.08f);
        setPlaying(true);
        requestReset();
    }

    void NBodyComponent::applyChaoticPreset()
    {
        setMaxBodies(1024);
        setGravitationalConstant(aiko::vec3(2.0f));
        setSoftening(0.05f);
        setTimeScale(1.0f);
        setInitialRadius(4.0f);
        setInitialSpeed(0.25f);
        setRenderScale(0.08f);
        setPlaying(true);
        requestReset();
    }

    void NBodyComponent::applyStressTestPreset()
    {
        setMaxBodies(32768);
        setGravitationalConstant(aiko::vec3(0.5f));
        setCentralMass(500.0f);
        setSoftening(0.2f);
        setTimeScale(1.0f);
        setInitialRadius(10.0f);
        setInitialSpeed(0.4f);
        setRenderScale(0.8f);
        setPlaying(true);
        requestReset();
    }
}
