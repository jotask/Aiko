#pragma once

#include "modules/base_module.h"

namespace aiko
{

    class DisplayModule;
    class AssetsManagerModule;
    class Camera;
    class Mesh;
    class Shader;

    class RenderModule : public BaseModule
    {
    
    public:

        RenderModule(Aiko* aiko);
        virtual ~RenderModule() = default;

        void setMainCamera(const Camera* camera); // TEMPORAL, this should be removed from here
        void submitLights(const AmbientLight& ambient, const std::vector<LightData>& data);

        // TODO : Temporal, we should just proxy to renderer
        AikoRenderer& getRenderer() { return *m_renderer.get(); }

    protected:


        virtual void connect(ModuleConnector*) override;
        virtual void init() override;
        virtual void update() override;
        virtual void beginFrame() override;
        virtual void endFrame() override;
        virtual void dispose() override;

    private:

        AssetsManagerModule* m_assetManager;
        const Camera* m_mainCamera;

        std::vector<InstanceItem> m_instances;

        AikoUPtr<AikoRenderer> m_renderer;

    };

}
