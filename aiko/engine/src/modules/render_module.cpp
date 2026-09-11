#include "render_module.h"

#include "aiko.h"
#include "assets_manager_module.h"
#include "modules/display_module.h"
#include "models/camera.h"
#include "modules/module_connector.h"

#include <aiko_renderer.h>

namespace aiko
{
    RenderModule::RenderModule(Aiko* aiko)
        : BaseModule(aiko)
        , m_mainCamera(nullptr)
        , m_renderer(nullptr)
    {
    }

    void RenderModule::setClearColor(Color color)
    {
        m_renderer->setClearColor(color);
    }

    void RenderModule::setMainCamera(const Camera* camera)
    {
        m_mainCamera = camera;
    }

    void RenderModule::submitLights(const AmbientLight& ambient, const vector<LightData>& data)
    {
        m_renderer->submit(ambient, data);
    }

    Mesh& RenderModule::getMesh(const AssetId& id)
    {
        return m_renderer->resources().getMesh(id);
    }

    Model& RenderModule::getModel(const AssetId& id)
    {
        return m_renderer->resources().getModel(id);
    }

    ComputeShader& RenderModule::getComputeShader(const AssetId& id)
    {
        return m_renderer->resources().getComputeShader(id);
    }

    void RenderModule::submit(const Transform& transform, const Mesh& mesh, const Material& material)
    {
        m_renderer->submit(transform, mesh, material);
    }

    void RenderModule::submitInstanced(const Mesh& mesh, const Material& material, const InstanceData* instances, u32 instanceCount)
    {
        m_renderer->submit(
            mesh,
            material,
            instances,
            instanceCount,
            sizeof(InstanceData)
        );
    }

    void RenderModule::enqueueCompute(const ComputePass& pass)
    {
        m_renderer->enqueueCompute(pass);
    }

    void RenderModule::drawVerticesGpu(const GpuVertexDrawDesc& desc)
    {
        m_renderer->drawVerticesGpu(desc);
    }

    void RenderModule::drawMeshInstancedGpu(const GpuInstanceDrawDesc& desc)
    {
        m_renderer->drawMeshInstancedGpu(desc);
    }

    void RenderModule::submitTransient(const Transform& transform, const Material& material, const MeshAsset& meshAsset, TransientTopology topology)
    {
        m_renderer->submitTransient(transform, material, meshAsset, topology);
    }

    void RenderModule::requestReadback(const ComputeReadbackRequest& req)
    {
        m_renderer->requestReadback(req);
    }

    bool RenderModule::pollReadback(ComputeReadbackResult& out)
    {
        return m_renderer->pollReadback(out);
    }

    void RenderModule::renderToTarget(const Camera& camera, RenderTarget& target)
    {
        m_renderer->renderToTarget(camera, target);
    }

    const FrameBuffer& RenderModule::getTargetTexture() const
    {
        return m_renderer->sceneRenderTarget().frameBuffer();
    }

    void RenderModule::updateTexture(const AssetId& id)
    {
        m_renderer->resources().updateTexture(id);
    }

    void RenderModule::unloadMesh(const AssetId& id)
    {
        m_renderer->resources().unloadMesh(id);
    }

    void RenderModule::unloadModel(const AssetId& id)
    {
        m_renderer->resources().unloadModel(id);
    }

    void RenderModule::unloadShader(const AssetId& id)
    {
        m_renderer->resources().unloadShader(id);
    }

    void RenderModule::connect(ModuleConnector* moduleConnector)
    {
        BIND_MODULE_REQUIRED(DisplayModule, moduleConnector, m_displayModule);
        BIND_MODULE_REQUIRED(AssetsManagerModule, moduleConnector, m_assetManager);
    }

    void RenderModule::init()
    {

        AssetManager* assetManager = m_assetManager->getManager();

        const ivec2 size = m_displayModule->getDisplaySize();
        const RenderSurfaceDesc surface =
        {
            .nativeWindowHandle = m_displayModule->getNativeWindow(),
            .width = static_cast<u32>(size.x),
            .height = static_cast<u32>(size.y),
        };

        m_renderer = std::make_unique<AikoRenderer>(*assetManager);
        m_renderer->init(getAiko()->getConfig().renderer, surface);
    }

    void RenderModule::update()
    {
        BaseModule::update();
    }

    void RenderModule::beginFrame()
    {
        m_instances.clear();
        m_renderer->beginFrame();
    }

    void RenderModule::endFrame()
    {
        AIKO_ASSERT(m_mainCamera != nullptr, "Main camera not set. Forgot to call set Main camera?");
        m_renderer->render(*m_mainCamera);
        m_renderer->endFrame();
        m_instances.clear();
    }

    void RenderModule::preDispose()
    {
        m_renderer->waitIdle();
    }

    void RenderModule::dispose()
    {
        m_renderer->dispose();
    }

}
