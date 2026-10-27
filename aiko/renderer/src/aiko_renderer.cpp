#include "aiko_renderer.h"

#include <events/events.hpp>
#include <intrumentor/profiler.h>
#include <iresource_resolver.h>

#include "display/display_events.hpp"

#include "renderer/Irenderdevice.h"
#include "assets/types/material_asset.h"
#include "assets/types/mesh_asset.h"
#include "core/transform.h"
#include "metadata/material_instance.h"
#include "models/camera.h"
#include "models/mesh.h"
#include "models/texture.h"
#include "renderer/frame_uniforms.h"
#include "types/render_config.h"
#include "render_factory.h"
#include "time/time.h"
#include "imgui/aiko_imgui.h"
#include <assets/mesh_tangents.h>

#include <cstdlib>
#include <utility>

namespace aiko
{
    AikoRenderer::AikoRenderer(IAssetProvider& assets, IAssetRegistry& registry)
        : m_resources(assets)
        , m_renderer(renderer::RendererFactory::createRenderDevice(&m_resources))
        , m_assetRegistry(registry)
    {

    }

    AikoRenderer::~AikoRenderer()
    {
        EventSystem::it().unbindAll(this);
    }

    void AikoRenderer::init(const RendererConfig& config, const RenderSurfaceDesc& surface, const IResourceResolver& resolver)
    {
        m_resourceResolver = &resolver;

        AIKO_ASSERT(surface.nativeWindowHandle != nullptr, "No window created!");

        m_renderSurface =
        {
            static_cast<int>(surface.width),
            static_cast<int>(surface.height)
        };

        const DeviceInitDesc description =
        {
            .nativeWindowHandle = surface.nativeWindowHandle,
            .width = surface.width,
            .height = surface.height,
            .vsync = config.vsync,
            .resources = &resolver,
        };

        if (m_renderer->init(description) == false)
        {
            AIKO_ASSERT(false, "Renderer not initialized");
            std::abort();
        }

        initializeBuiltinShaders();

        // Create scene render target
        if (m_sceneTarget.isValid() == true)
        {
            m_sceneTarget.unload();
        }

        m_sceneTarget.create(surface.width, surface.height, TextureFormat::RGBA16F);
        AIKO_ASSERT(m_sceneTarget.isValid(), "Failed to create HDR scene target");

        m_shadowRenderer.init(m_assetRegistry);
        AIKO_ASSERT(m_shadowRenderer.isValid(), "Failed to create Shadow renderer");

        m_screenPresenter.init();
        AIKO_ASSERT(m_screenPresenter.isValid(), "Screen presenter invalid");

        // bind to on window resize
        EventSystem::it().bind<WindowResizeEvent>(this, &AikoRenderer::onWindowResize);

        m_imgui.init(static_cast<GLFWwindow*>(surface.nativeWindowHandle));

    }

    void AikoRenderer::beginFrame()
    {
        AIKO_FUNCTION_PROFILE;

        if (m_pendingSurfaceResize.has_value())
        {
            m_renderSurface = *m_pendingSurfaceResize;
            m_renderer->resize(m_renderSurface.x, m_renderSurface.y);
            m_sceneTarget.resize(m_renderSurface.x, m_renderSurface.y);
            m_postProcessing.resize(static_cast<u32>(m_renderSurface.x), static_cast<u32>(m_renderSurface.y));
            m_pendingSurfaceResize.reset();
        }

        m_renderer->beginFrame();

        ComputeReadbackResult completed;

        while (m_renderer->pollReadback(completed))
        {
            const auto it = m_readbacks.find(completed.id);

            if (it != m_readbacks.end())
            {
                it->second = std::move(completed);
            }
        }

        m_renderQueue.clear();
        m_sceneRenderRequests.clear();
        m_computeQueue.clear();
        m_gpuInstanceSorter.beginFrame();

        m_frameMaterialCache.clear();
        m_frameInstancedMaterials.clear();
        m_frameMaterials.clear();
        m_frameTransientGeometries.clear();
        m_lights.clear();
        m_uiDrawList.clear();
        m_uiRenderer.beginFrame();
        m_imgui.beginFrame(m_renderSurface.x, m_renderSurface.y);
    }

    void AikoRenderer::endFrame()
    {
        AIKO_FUNCTION_PROFILE;
        m_renderer->endFrame();
        m_renderer->present();
    }

    void AikoRenderer::dispose()
    {
        m_renderer->waitIdle();
        m_gpuInstanceSorter.dispose();
        m_billboardMesh.unload();
        m_imgui.dispose();
        m_frameTransientGeometries.clear();
        m_screenPresenter.dispose();
        m_postProcessing.dispose();
        m_shadowRenderer.dispose();
        m_sceneTarget.unload();
        m_resources.clear();
        m_renderer->shutdown();
        m_readbacks.clear();
    }

    void AikoRenderer::setClearColor(Color color)
    {
        m_clearColor = color;
    }

    bool AikoRenderer::isFrameActive() const
    {
        return m_renderer->isFrameActive();
    }

    void AikoRenderer::submit(const AmbientLight& ambient, const vector<LightData>& data)
    {
        AIKO_FUNCTION_PROFILE
        m_ambientLight = ambient;
        m_lights.insert(m_lights.end(), data.begin(), data.end());
    }

    void AikoRenderer::submit(const LightData& light)
    {
        m_lights.push_back(light);
    }

    void AikoRenderer::submit(const Transform& transform, const Mesh& mesh, const Material& material)
    {
        AIKO_FUNCTION_PROFILE
        if (!mesh.isValid())
        {
            return;
        }
        m_frameMaterials.push_back(material);
        m_renderQueue.submit(mesh, m_frameMaterials.back(), transform.getWorldMatrix());
    }

    void AikoRenderer::submit(const Mesh& mesh, const Material& material, const void* data, uint32_t instanceCount, uint16_t stride)
    {
        AIKO_FUNCTION_PROFILE
        if (!mesh.isValid() || data == nullptr || instanceCount == 0 || stride == 0)
        {
            return;
        }
        Material& staged = stageInstancedMaterial(material);
        m_renderQueue.submitInstanced(mesh, staged, data, instanceCount, stride);
    }

    void AikoRenderer::submit(const Transform& transform, const Mesh& mesh, const MaterialAsset& materialAsset, const MaterialInstance& materialInstance)
    {
        AIKO_FUNCTION_PROFILE
        if (!mesh.isValid())
        {
            return;
        }
        Material& stagedMaterial = stageMaterial(materialAsset, materialInstance);
        m_renderQueue.submit(mesh, stagedMaterial, transform.getWorldMatrix());
    }

    void AikoRenderer::submitFullscreen(const Material& material)
    {
        AIKO_FUNCTION_PROFILE
        m_frameMaterials.push_back(material);
        m_renderQueue.submitFullscreen(m_frameMaterials.back());
    }

    void AikoRenderer::submitTransient(const Transform& transform, const Material& material, const MeshAsset& meshAsset, TransientTopology topology)
    {
        AIKO_FUNCTION_PROFILE
        m_frameMaterials.push_back(material);
        TransientDrawDesc draw{};
        draw.mtx = transform.getWorldMatrix();
        draw.material = &m_frameMaterials.back();
        draw.geometry = &stageTransientGeometry(meshAsset, topology);
        m_renderQueue.submitTransient(std::move(draw));
    }

    void AikoRenderer::enqueueCompute(const ComputePass& pass)
    {
        m_computeQueue.push_back(pass);
    }

    ReadbackId AikoRenderer::requestReadback(const ComputeReadbackRequest& req)
    {
        if (m_nextReadbackId == InvalidReadbackId)
        {
            std::abort();
        }

        const ReadbackId id = m_nextReadbackId++;

        ComputeReadbackRequest submitted = req;
        submitted.id = id;

        m_readbacks.emplace(id, ComputeReadbackResult{});

        try
        {
            m_renderer->requestReadback(submitted);
        }
        catch (...)
        {
            m_readbacks.erase(id);
            throw;
        }

        return id;
    }

    bool AikoRenderer::pollReadback(ReadbackId id, ComputeReadbackResult& out)
    {
        const auto it = m_readbacks.find(id);

        if (it == m_readbacks.end() || it->second.status == ReadbackStatus::Pending)
        {
            return false;
        }

        out = std::move(it->second);
        m_readbacks.erase(it);
        return true;
    }

    void AikoRenderer::cancelReadback(ReadbackId id)
    {
        m_readbacks.erase(id);
    }

    void AikoRenderer::drawMeshInstancedGpu(const GpuInstanceDrawDesc& desc)
    {
        AIKO_ASSERT(desc.material != nullptr, "GPU instance draw has no material");
        AIKO_ASSERT(desc.mesh != nullptr, "GPU instance draw has no mesh");
        if (!desc.mesh->isValid())
        {
            return;
        }
        m_frameMaterials.push_back(*desc.material);
        GpuInstanceDrawDesc staged = desc;
        staged.material = &m_frameMaterials.back();
        m_renderQueue.submitGpuInstances(staged);
    }

    void AikoRenderer::drawBillboards(const GpuBillboardDrawDesc& desc)
    {
        if (desc.instanceCount == 0)
        {
            return;
        }

        AIKO_ASSERT(desc.material != nullptr, "GPU billboard draw has no material");
        AIKO_ASSERT(desc.positionBuffer != nullptr, "GPU billboard draw has no position buffer");
        AIKO_ASSERT(desc.positionBuffer->isValid(), "Invalid GPU billboard position buffer");

        m_frameMaterials.push_back(*desc.material);
        Material& material = m_frameMaterials.back();

        const bool transparent = material.m_renderState.blend;

        if (material.m_shaderId == InvalidAssetId)
        {
            material.m_shaderId = transparent ? m_billboardTransparentShaderId : m_billboardOpaqueShaderId;

            AIKO_ASSERT(material.m_shaderId != InvalidAssetId, "Default billboard shaders have not been configured");
        }

        GpuInstanceDrawDesc draw{};
        draw.mesh = &resolveBillboardMesh();
        draw.material = &material;
        draw.readBuffers = {
            {7, desc.positionBuffer}
        };
        draw.instanceCount = desc.instanceCount;
        draw.transparencySortPosition = desc.transparencySortPosition;

        if (transparent)
        {
            draw.transparencyPositions = desc.positionBuffer;
        }

        m_renderQueue.submitGpuInstances(draw);
    }

    void AikoRenderer::drawVerticesGpu(const GpuVertexDrawDesc& desc)
    {
        AIKO_ASSERT(desc.material != nullptr, "GPU vertex draw has no material");
        m_frameMaterials.push_back(*desc.material);
        GpuVertexDrawDesc staged = desc;
        staged.material = &m_frameMaterials.back();
        m_renderQueue.submitGpuVertices(staged);
    }

    void AikoRenderer::render(const Camera& camera)
    {
        render(&camera);
    }

    void AikoRenderer::render(const Camera* camera)
    {
        AIKO_FUNCTION_PROFILE

        Camera fallbackCamera;
        fallbackCamera.setCameraType(Camera::Perspective);

        const Camera& frameCamera = camera != nullptr ? *camera : fallbackCamera;

        const renderer::FrameData frameData = buildSceneFrameData(frameCamera, m_sceneTarget.size());

        m_renderer->bindFrame(COMPUTE_VIEW, frameData);
        executeComputePasses(camera != nullptr ? &frameData.view : nullptr);

        for (SceneRenderRequest& request : m_sceneRenderRequests)
        {
            AIKO_ASSERT(request.target != nullptr, "Scene render request has no target");
            AIKO_ASSERT(request.target->isValid(), "Scene render request target is invalid");

            const renderer::FrameData requestFrame = buildSceneFrameData(request.camera, request.target->size());
            const PreparedScenePass requestPass = request.queue.buildScenePass(requestFrame.view);

            submitScenePass(requestFrame, requestPass, *request.target);
        }

        if (camera != nullptr)
        {
            const PreparedScenePass scenePass = m_renderQueue.buildScenePass(frameData.view);
            m_shadowRenderer.render(*m_renderer, frameData, scenePass);
            submitScenePass(frameData, scenePass, m_sceneTarget);
        }
        else
        {
            const PreparedScenePass emptyPass{};
            submitScenePass(frameData, emptyPass, m_sceneTarget);
        }

        const Texture& finalTexture = m_postProcessing.execute(*m_renderer, m_frameMaterials, m_sceneTarget);

        submitPresentPass(finalTexture);
        submitUiPass();
        submitImguiPass();

    }

    void AikoRenderer::setDebugTexture(const Texture* texture)
    {
        m_debugTexture = texture;
    }

    void AikoRenderer::renderToTarget(const Camera& camera, RenderTarget& target)
    {
        AIKO_ASSERT(target.isValid(), "Cannot render to an invalid RenderTarget");
        m_sceneRenderRequests.push_back(
            SceneRenderRequest
            {
                .camera = camera,
                .target = &target,
                .queue = m_renderQueue,
            }
        );
    }

    void AikoRenderer::waitIdle()
    {
        m_renderer->waitIdle();
    }

    ImguiTextureId AikoRenderer::imguiTextureId(const Texture& texture, const SamplerState& sampler)
    {
        return m_imgui.textureId(texture, sampler);
    }

    void AikoRenderer::drawUiRect(const vec2& position, const vec2& size, Color color, float cornerRadius, float borderThickness, Color borderColor)
    {
        m_uiDrawList.addRect(
            {
                .position = position,
                .size = size
            },
            color,
            cornerRadius,
            borderThickness,
            borderColor);
    }

    void AikoRenderer::drawUiImage(AssetId textureId, const vec2& position, const vec2& size, Color tint, float cornerRadius, float borderThickness, Color borderColor)
    {
        m_uiDrawList.addImage(
            {
                .position = position,
                .size = size
            },
            textureId,
            tint,
            cornerRadius,
            borderThickness,
            borderColor);
    }

    void AikoRenderer::drawUiImage(AssetId textureId, const TextureRegion& region, const vec2& position, const vec2& size, Color tint, float cornerRadius, float borderThickness, Color borderColor)
    {
        m_uiDrawList.addImage(
            {
                .position = position,
                .size = size
            },
            textureId,
            region,
            tint,
            cornerRadius,
            borderThickness,
            borderColor);
    }

    void AikoRenderer::pushUiClipRect(const vec2& position, const vec2& size)
    {
        m_uiDrawList.pushClipRect(
        {
            .position = position,
            .size = size
        });
    }

    void AikoRenderer::popUiClipRect()
    {
        m_uiDrawList.popClipRect();
    }

    void AikoRenderer::onWindowResize(const WindowResizeEvent& event)
    {
        if (event.width <= 0 || event.height <= 0)
        {
            return;
        }
        m_pendingSurfaceResize =
        {
            event.width,
            event.height
        };
    }

    void AikoRenderer::initializeBuiltinShaders()
    {
        const AssetId uiShader = m_assetRegistry.registerShader("renderer://ui.vs", "renderer://ui.fs");
        AIKO_ASSERT(uiShader != InvalidAssetId, "Failed to register built-in UI shader");

        m_uiRenderer.setShader(uiShader);

        m_billboardOpaqueShaderId = m_assetRegistry.registerShader("renderer://gpu_billboard_default.vs", "renderer://model.fs");
        AIKO_ASSERT(m_billboardOpaqueShaderId != InvalidAssetId, "Failed to register opaque billboard shader");

        m_billboardTransparentShaderId = m_assetRegistry.registerShader("renderer://gpu_billboard_default_sorted.vs", "renderer://model.fs");
        AIKO_ASSERT(m_billboardTransparentShaderId != InvalidAssetId, "Failed to register transparent billboard shader");
    }

    renderer::FrameData AikoRenderer::buildSceneFrameData(const Camera& camera, const ivec2& targetSize) const
    {
        AIKO_FUNCTION_PROFILE
        return
        {
            .view = camera.getViewMatrix(),
            .projection = camera.getProjectionMatrix(targetSize),
            .cameraPosition = camera.position,
            .time = static_cast<float>(Time::it().secondSinceStart()),
            .deltaTime = Time::it().getDeltaTime(),
            .ambient = m_ambientLight,
            .lights = m_lights,
        };
    }

    void AikoRenderer::executeComputePasses(const mat4* mainView)
    {
        AIKO_FUNCTION_PROFILE

        const renderer::PassDescription computePass{
            .width = 1,
            .height = 1
        };

        m_renderer->beginPass(COMPUTE_VIEW, computePass, nullptr);

        for (const ComputePass& pass : m_computeQueue)
        {
            m_renderer->execute(COMPUTE_VIEW, pass);
        }

        for (SceneRenderRequest& request : m_sceneRenderRequests)
        {
            AIKO_ASSERT(request.target != nullptr, "Scene render request has no target");
            AIKO_ASSERT(request.target->isValid(), "Scene render request target is invalid");

            const mat4 view = request.camera.getViewMatrix();

            request.queue.prepareGpuInstanceSorts(m_gpuInstanceSorter, *m_renderer, view, *m_resourceResolver);
        }

        if (mainView != nullptr)
        {
            m_renderQueue.prepareGpuInstanceSorts(m_gpuInstanceSorter, *m_renderer, *mainView, *m_resourceResolver);
        }

        m_renderer->endPass();
    }

    void AikoRenderer::submitScenePass(const renderer::FrameData& frameData, const PreparedScenePass& passData, const RenderTarget& target)
    {
        AIKO_FUNCTION_PROFILE
        const ivec2 size = target.size();
        const renderer::PassDescription pass =
        {
            .width = static_cast<u32>(size.x),
            .height = static_cast<u32>(size.y),
            .colorLoadOp = renderer::AttachmentLoadOp::Clear,
            .depthLoadOp = renderer::AttachmentLoadOp::Clear,
            .clear = m_clearColor
        };

        const FrameBuffer& frameBuffer = target.frameBuffer();

        auto prepareMaterial = [&](const Material* material)
        {
            if (material != nullptr)
            {
                m_renderer->prepareMaterial(*material);
            }
        };

        for (const Material* material : passData.fullscreen)
        {
            prepareMaterial(material);
        }

        for (const GpuInstanceDrawDesc* desc : passData.gpuInstances)
        {
            if (desc == nullptr)
            {
                continue;
            }
            prepareMaterial(desc->material);
            m_renderer->prepareGpuReadBuffers(desc->readBuffers);
        }

        for (const GpuBillboardDrawDesc* desc : passData.gpuBillboards)
        {
            if (desc == nullptr)
            {
                continue;
            }

            prepareMaterial(desc->material);

            m_renderer->prepareGpuReadBuffers(
                {
                    {
                        .slot = 7,
                        .buffer = desc->positionBuffer,
                    }
                });
        }

        for (const GpuVertexDrawDesc* desc : passData.gpuVertices)
        {
            if (desc == nullptr)
            {
                continue;
            }

            prepareMaterial(desc->material);

            if (desc->vertexBuffer != nullptr)
            {
                m_renderer->prepareVertexBuffer(*desc->vertexBuffer);
            }

            if (desc->indexBuffer != nullptr)
            {
                m_renderer->prepareIndexBuffer(*desc->indexBuffer);
            }

            if (desc->indirectBuffer != nullptr)
            {
                m_renderer->prepareIndirectBuffer(*desc->indirectBuffer);
            }
        }

        for (const PreparedRenderPacket& packet : passData.opaque)
        {
            prepareMaterial(packet.draw.material);
        }

        for (const PreparedInstancedPacket& packet : passData.instanced)
        {
            prepareMaterial(packet.draw.material);
        }

        for (const PreparedTransientPacket& packet : passData.transient)
        {
            if (packet.item != nullptr)
            {
                prepareMaterial(packet.item->material);
            }
        }

        for (const PreparedTransparentPacket& packet : passData.transparent)
        {
            if (packet.gpuVertex != nullptr || packet.gpuInstance != nullptr || packet.gpuBillboard != nullptr)
            {
                continue;
            }

            if (packet.transient != nullptr)
            {
                prepareMaterial(packet.transient->material);
            }
            else if (packet.instanced.data != nullptr)
            {
                prepareMaterial(packet.instanced.material);
            }
            else
            {
                prepareMaterial(packet.draw.material);
            }
        }

        m_renderer->beginPass(SCENE_VIEW, pass, &frameBuffer);
        m_renderer->bindFrame(SCENE_VIEW, frameData);

        {
            for (const Material* material : passData.fullscreen)
            {
                AIKO_ASSERT(material != nullptr, "Prepared fullscreen draw has null material");
                m_renderer->submitFullscreen(SCENE_VIEW, *material);
            }
        }

        {
            for (const GpuVertexDrawDesc* desc : passData.gpuVertices)
            {
                if (desc != nullptr && desc->material != nullptr && desc->material->m_renderState.blend == false)
                {
                    m_renderer->submitGpuVertices(SCENE_VIEW, *desc);
                }
            }
        }

        {
            for (const GpuInstanceDrawDesc* desc : passData.gpuInstances)
            {
                if (desc != nullptr && desc->material != nullptr && desc->material->m_renderState.blend == false)
                {
                    m_renderer->submitGpuInstanced(SCENE_VIEW, *desc);
                }
            }
        }

        {
            for (const GpuBillboardDrawDesc* desc : passData.gpuBillboards)
            {
                if (desc != nullptr && desc->material != nullptr && desc->material->m_renderState.blend == false)
                {
                    m_renderer->submitBillboards(SCENE_VIEW, *desc);
                }
            }
        }

        {
            for (const PreparedRenderPacket& packet : passData.opaque)
            {
                AIKO_ASSERT(packet.draw.mesh != nullptr, "Prepared opaque packet has null mesh");
                AIKO_ASSERT(packet.draw.material != nullptr, "Prepared opaque packet has null material");

                m_renderer->submitMesh(SCENE_VIEW, packet.draw);
            }
        }

        {
            for (const PreparedInstancedPacket& packet : passData.instanced)
            {
                AIKO_ASSERT(packet.draw.mesh != nullptr, "Prepared instanced packet has null mesh");
                AIKO_ASSERT(packet.draw.material != nullptr, "Prepared instanced packet has null material");
                AIKO_ASSERT(packet.draw.data != nullptr, "Prepared instanced packet has null data");

                m_renderer->submitInstanced(SCENE_VIEW, packet.draw);
            }
        }

        {
            for (const PreparedTransientPacket& packet : passData.transient)
            {
                AIKO_ASSERT(packet.item != nullptr, "Prepared transient packet has null item");
                m_renderer->submitTransient(SCENE_VIEW, *packet.item);
            }
        }

        {
            for (const PreparedTransparentPacket& packet : passData.transparent)
            {
                if (packet.gpuVertex != nullptr)
                {
                    m_renderer->submitGpuVertices(SCENE_VIEW, *packet.gpuVertex);
                }
                else if (packet.gpuInstance != nullptr)
                {
                    m_renderer->submitGpuInstanced(SCENE_VIEW, *packet.gpuInstance);
                }
                else if (packet.gpuBillboard != nullptr)
                {
                    m_renderer->submitBillboards(SCENE_VIEW, *packet.gpuBillboard);
                }
                else if (packet.transient != nullptr)
                {
                    m_renderer->submitTransient(SCENE_VIEW, *packet.transient);
                }
                else if (packet.instanced.data != nullptr)
                {
                    m_renderer->submitInstanced(SCENE_VIEW, packet.instanced);
                }
                else
                {
                    m_renderer->submitMesh(SCENE_VIEW, packet.draw);
                }
            }
        }

        m_renderer->endPass();
    }

    void AikoRenderer::submitPresentPass(const Texture& sceneTexture)
    {
        AIKO_FUNCTION_PROFILE

        const renderer::PassDescription presentPass =
        {
            .width = static_cast<u32>(m_renderSurface.x),
            .height = static_cast<u32>(m_renderSurface.y),
            .colorLoadOp = renderer::AttachmentLoadOp::Clear,
            .depthLoadOp = renderer::AttachmentLoadOp::Clear,
        };

        const renderer::FrameData screenFrame =
        {
            .view = mat4(1.0f),
            .projection = mat4(1.0f),
        };

        const Texture* presentTexture = &sceneTexture;

        if (m_debugTexture != nullptr && m_debugTexture->isValid())
        {
            presentTexture = m_debugTexture;
        }

        AIKO_ASSERT(presentTexture->isValid(), "Present texture is invalid");

        m_renderer->prepareTextureForSampling(*presentTexture);

        m_renderer->beginPass(SCREEN_VIEW, presentPass, nullptr);
        m_renderer->bindFrame(SCREEN_VIEW, screenFrame);

        m_renderer->presentTextureToScreen(SCREEN_VIEW, m_screenPresenter.mesh(), *presentTexture);

        m_renderer->endPass();
    }

    void AikoRenderer::submitUiPass()
    {
        if (m_uiDrawList.empty())
        {
            return;
        }

        const float width = static_cast<float>(m_renderSurface.x);
        const float height = static_cast<float>(m_renderSurface.y);

        const renderer::PassDescription uiPass =
        {
            .width = static_cast<u32>(m_renderSurface.x),
            .height = static_cast<u32>(m_renderSurface.y),
            .colorLoadOp = renderer::AttachmentLoadOp::Load,
            .depthLoadOp = renderer::AttachmentLoadOp::DontCare,
        };

        const renderer::FrameData uiFrame =
        {
            .view = mat4(1.0f),
            .projection = math::ortho(
                0.0f,
                width,
                0.0f,
                height,
                -1.0f,
                1.0f),
        };

        m_renderer->prepareMaterial(m_uiRenderer.material()); // see below

        m_renderer->beginPass(UI_VIEW, uiPass, nullptr);
        m_renderer->bindFrame(UI_VIEW, uiFrame);

        m_uiRenderer.render(*m_renderer, m_uiDrawList, m_renderSurface);

        m_renderer->endPass();

    }

    void AikoRenderer::submitImguiPass()
    {
        const renderer::PassDescription imguiPass =
        {
            .width = static_cast<u32>(m_renderSurface.x),
            .height = static_cast<u32>(m_renderSurface.y),
            .colorLoadOp = renderer::AttachmentLoadOp::Load,
            .depthLoadOp = renderer::AttachmentLoadOp::DontCare,
        };

        m_renderer->beginPass(IMGUI_VIEW, imguiPass, nullptr);
        m_imgui.endFrame(m_renderSurface.x, m_renderSurface.y);
        m_renderer->endPass();
    }

    Material& AikoRenderer::stageMaterial(const MaterialAsset& materialAsset, const MaterialInstance& materialInstance)
    {
        AIKO_FUNCTION_PROFILE
        FrameMaterialKey key{};
        key.shaderId =
            materialInstance.shaderId != InvalidAssetId
                ? materialInstance.shaderId
                : materialAsset.shaderId;

        key.diffuseTextureId = materialAsset.diffuseTextureId;
        key.runtimeDiffuseTexture = materialInstance.runtimeDiffuseTexture;
        key.useVertexColor = materialAsset.useVertexColor;
        key.lit = materialAsset.lit;
        key.baseColor = materialAsset.baseColor;
        key.specularStrength = materialAsset.specularStrength;
        key.shininess = materialAsset.shininess;
        key.metallic = materialAsset.metallic;
        key.roughness = materialAsset.roughness;

        if (auto it = m_frameMaterialCache.find(key); it != m_frameMaterialCache.end())
        {
            AIKO_ASSERT(it->second != nullptr, "Cached frame material pointer is null");
            return *it->second;
        }

        m_frameMaterials.emplace_back();
        Material& material = m_frameMaterials.back();

        material.m_shaderId = key.shaderId;
        material.m_gpuInstanceBuffer = nullptr;

        material.m_useVertexColor = key.useVertexColor;
        material.m_lit = key.lit;
        material.m_baseColor = key.baseColor;
        material.setSpecular(key.specularStrength, key.shininess);
        material.setMetallic(key.metallic);
        material.setRoughness(key.roughness);

        if (key.runtimeDiffuseTexture != nullptr)
        {
            material.setTexture("u_texture", key.runtimeDiffuseTexture);
        }
        else if (key.diffuseTextureId != InvalidAssetId)
        {
            material.setTexture("u_texture", key.diffuseTextureId);
        }

        // Optional prewarm. Not a major perf win, but ensures shader exists before draw.
        if (material.m_shaderId != InvalidAssetId)
        {
            (void)m_resources.getShader(material.m_shaderId);
        }

        m_frameMaterialCache.emplace(key, &material);
        return material;
    }

    const TransientGeometry& AikoRenderer::stageTransientGeometry(const MeshAsset& meshAsset, TransientTopology topology)
    {

        AIKO_FUNCTION_PROFILE

        AIKO_ASSERT(meshAsset.m_tangents.empty() || meshAsset.m_tangents.size() == meshAsset.m_vertices.size(), "Transient mesh tangent count must match vertex count");

        vector<vec4> generatedTangents;

        if (topology == TransientTopology::Triangles && meshAsset.m_tangents.empty())
        {
            generatedTangents = mesh::calculateTangents(meshAsset);
        }

        const vector<vec4>& tangents = meshAsset.m_tangents.empty() ? generatedTangents : meshAsset.m_tangents;

        m_frameTransientGeometries.emplace_back();

        TransientGeometry& geometry = m_frameTransientGeometries.back();

        geometry.topology = topology;
        geometry.vertices.reserve(meshAsset.m_vertices.size());

        for (uint32_t i = 0; i < static_cast<uint32_t>(meshAsset.m_vertices.size()); ++i)
        {
            TransientVertex vertex{};

            vertex.position = meshAsset.m_vertices[i];

            if (i < meshAsset.m_textCoord.size())
            {
                vertex.uv = meshAsset.m_textCoord[i];
            }

            if (i < meshAsset.m_normals.size())
            {
                vertex.normal = meshAsset.m_normals[i];
            }

            if (i < meshAsset.m_colors.size())
            {
                vertex.color = meshAsset.m_colors[i];
            }
            else
            {
                vertex.color = WHITE;
            }

            if (!tangents.empty())
            {
                vertex.tangent = tangents[i];
            }

            geometry.vertices.push_back(vertex);

        }

        geometry.indices = meshAsset.m_indices;

        return geometry;
    }

    Material& AikoRenderer::stageInstancedMaterial(const Material& material)
    {
        auto& candidates = m_frameInstancedMaterials[material.id()];

        for (Material* candidate : candidates)
        {
            if (candidate->hasSameState(material))
            {
                return *candidate;
            }
        }

        m_frameMaterials.push_back(material);
        Material& staged = m_frameMaterials.back();

        candidates.push_back(&staged);
        return staged;
    }

    Mesh& AikoRenderer::resolveBillboardMesh()
    {
        if (!m_billboardMesh.isValid())
        {
            MeshAsset mesh{};

            mesh.m_vertices = {
                {-0.5f, -0.5f, 0.0f},
                { 0.5f, -0.5f, 0.0f},
                { 0.5f,  0.5f, 0.0f},
                {-0.5f,  0.5f, 0.0f}
            };

            mesh.m_textCoord = {
                {0.0f, 1.0f},
                {1.0f, 1.0f},
                {1.0f, 0.0f},
                {0.0f, 0.0f}
            };

            mesh.m_normals.assign(4, vec3{0.0f, 0.0f, 1.0f});
            mesh.m_colors.assign(4, WHITE);
            mesh.m_indices = {0, 1, 2, 0, 2, 3};

            m_billboardMesh.upload(mesh);
        }

        return m_billboardMesh;
    }
}
