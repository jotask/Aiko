#pragma once

#include "modules/base_module.h"

namespace aiko
{

    class Camera;
    class DisplayModule;
    class Mesh;
    class Shader;
    class Transform;

    class RenderModule : public BaseModule
    {
    
    public:

        RenderModule(Aiko* aiko);
        virtual ~RenderModule() = default;

        void setMainCamera(const Camera* camera); // TEMPORAL, this should be removed from here
        void submitLights(const AmbientLight& ambient, const std::vector<LightData>& data);

    protected:

        virtual void init() override;
        virtual void update() override;
        virtual void render() override;
        virtual void beginFrame() override;
        virtual void endFrame() override;
        virtual void dispose() override;

    private:

        const Camera* m_mainCamera;

        std::vector<InstanceItem> m_instances;

        // TEMP: compute validation (remove later when SceneSystem owns compute)
        bool m_computeInit = false;
        ComputeShader m_particlesCS;
        ComputeBuffer m_posBuffer;
        ComputeBuffer m_velBuffer;
        Texture m_debugOut;
        u32 m_particleCount = 16384;

        // TEMP readback validation
        bool m_readbackRequested = false;

    };

}
