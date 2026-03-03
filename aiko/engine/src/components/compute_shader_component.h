#pragma once

#include "models/component.h"

namespace aiko
{

    class ComputeShaderComponent : public Component, public IRender
    {
    public:
        ComputeShaderComponent();
        virtual ~ComputeShaderComponent() = default;

        virtual void init() override;
        virtual void render() override;

    protected:

        void load(string computer_shader);

    private:

        void refreshAll();

        // TEMP: compute validation (remove later when SceneSystem owns compute)
        bool m_computeInit = false;
        ComputeShader m_particlesCS;
        ComputeBuffer m_posBuffer;
        ComputeBuffer m_velBuffer;
        Texture m_debugOut;
        const u32 m_particleCount = 16384;

        // GPU mesh particle draw (NEW)
        bool m_meshParticlesInit = false;
        Mesh m_particleMesh;
        Material m_particleMeshMaterial;

        bool m_needInitDispatch;
        ComputeShader m_particlesInitCS;
        ComputeBuffer m_seedPos;
        ComputeBuffer m_seedVel;

        // TEMP readback validation
        bool m_readbackRequested = false;

    };

}
