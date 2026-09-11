#pragma once

#include <assets/asset_id.h>
#include "modules/base_module.h"
#include "models/light.h"

namespace aiko
{

    class DisplayModule;
    class AssetsManagerModule;
    class AikoRenderer;
    class Camera;
    class Mesh;
    class Shader;

    class RenderModule : public BaseModule
    {
    
    public:

        RenderModule(Aiko* aiko);
        virtual ~RenderModule() = default;

        void setClearColor(Color color);

        void setMainCamera(const Camera* camera);
        void submitLights(const AmbientLight& ambient, const vector<LightData>& data);

        Mesh& getMesh(const AssetId& id);
        Model& getModel(const AssetId& id);
        ComputeShader& getComputeShader(const AssetId& id);

        void submit(const Transform& transform, const Mesh& mesh, const Material& material);
        void submitInstanced(const Mesh& mesh, const Material& material, const InstanceData* instances, u32 instanceCount);

        void enqueueCompute(const ComputePass& pass);

        void drawVerticesGpu(const GpuVertexDrawDesc& desc);
        void drawMeshInstancedGpu(const GpuInstanceDrawDesc& desc);

        void submitTransient(const Transform& transform, const Material& material, const MeshAsset& meshAsset, TransientTopology topology);

        void requestReadback(const ComputeReadbackRequest& req);
        bool pollReadback(ComputeReadbackResult& out);

        void renderToTarget(const Camera& camera, RenderTarget& target);
        const FrameBuffer& getTargetTexture() const;

        void updateTexture(const AssetId& id);
        void unloadMesh(const AssetId& id);
        void unloadModel(const AssetId& id);
        void unloadShader(const AssetId& id);

        ImguiTextureId getTargetTextureId() const;

    protected:


        virtual void connect(ModuleConnector*) override;
        virtual void init() override;
        virtual void update() override;
        virtual void beginFrame() override;
        virtual void endFrame() override;
        virtual void preDispose() override;
        virtual void dispose() override;

    private:

        DisplayModule* m_displayModule = nullptr;
        AssetsManagerModule* m_assetManager = nullptr;
        const Camera* m_mainCamera = nullptr;

        vector<InstanceItem> m_instances;

        AikoUPtr<AikoRenderer> m_renderer;

    };

}
