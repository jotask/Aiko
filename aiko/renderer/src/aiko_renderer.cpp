#include "aiko_renderer.h"

#include <events/events.hpp>
#include <intrumentor/profiler.h>

#include "display/display_events.hpp"

#include "render_factory.h"
#include "core/transform.h"
#include "time/time.h"
#include "imgui/aiko_imgui.h"
#include "models/camera.h"

#include <unordered_set>

namespace aiko
{
    AikoRenderer::AikoRenderer(IAssetProvider& assets, const AssetId& passthroughShaderId)
        : m_resources(assets)
        , m_renderer(renderer::RendererFactory::createRenderDevice(&m_resources))
        , m_passthroughShaderId(passthroughShaderId)
    {

    }

    void AikoRenderer::init(const RendererConfig& config, const RenderSurfaceDesc& surface)
    {

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
        };

        if (m_renderer->init(description) == false)
        {
            AIKO_ASSERT(false, "Renderer not initialized");
            std::abort();
        }

        // Create Screen fbo
        if (m_screenFbo.isValid() == true)
        {
            m_screenFbo.unload();
        }
        AIKO_ASSERT(m_passthroughShaderId != InvalidAssetId, "ScreenFbo shader invalid!");
        Material screenFboMaterial = {};
        screenFboMaterial.m_shaderId = m_passthroughShaderId;
        screenFboMaterial.m_useVertexColor = false;
        screenFboMaterial.m_lit = false;
        screenFboMaterial.m_baseColor = WHITE;
        m_screenFbo.setMaterial(std::move(screenFboMaterial));

        m_screenFbo.create(surface.width, surface.height);

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
            m_screenFbo.resize(m_renderSurface.x, m_renderSurface.y);
            m_pendingSurfaceResize.reset();
        }

        m_renderer->beginFrame();

        m_frameMaterials.clear();
        m_frameMaterialCache.clear();
        m_queue.clear();
        m_transientQueue.clear();
        m_instancedQueue.clear();
        m_instanceDataArena.clear();
        m_gpuBillboardQueue.clear();
        m_mergedInstanceDataArena.clear();
        m_computeQueue.clear();
        m_gpuInstanceDraws.clear();
        m_gpuVertexDraws.clear();
        m_lights.clear();
        m_imgui.beginFrame(m_renderSurface.x, m_renderSurface.y);
    }

    void AikoRenderer::endFrame()
    {
        AIKO_FUNCTION_PROFILE;
        m_renderer->endFrame();
        m_renderer->present();
        AIKO_FRAME_MARK
    }

    void AikoRenderer::dispose()
    {
        m_imgui.dispose();
        m_transientGeometryCache.clear();
        m_screenFbo.unload();
        m_resources.clear();
        m_renderer->shutdown();
    }

    void AikoRenderer::setClearColor(Color color)
    {
        m_clearColor = color;
    }

    void AikoRenderer::submit(const AmbientLight& ambient, const vector<LightData>& data)
    {
        AIKO_FUNCTION_PROFILE
        m_ambientLight = ambient;
        m_lights.insert(m_lights.end(), data.begin(), data.end());
    }

    void AikoRenderer::submit(const Transform& transform, const Mesh& mesh, const Material& material)
    {
        AIKO_FUNCTION_PROFILE
        RenderItem item =
        {
            .mesh = &mesh,
            .material = &material,
            .transform = transform.getWorldMatrix()
        };
        m_queue.push_back(item);
    }

    void AikoRenderer::submit(const Mesh& mesh, const Material& material, const void* data, uint32_t instanceCount, uint16_t stride)
    {
        AIKO_FUNCTION_PROFILE
        if (data == nullptr || instanceCount == 0 || stride == 0)
        {
            return;
        }

        const size_t byteCount = size_t(instanceCount) * size_t(stride);
        if (byteCount == 0)
        {
            return;
        }

        const size_t offset = m_instanceDataArena.size();
        m_instanceDataArena.resize(offset + byteCount);
        memcpy(m_instanceDataArena.data() + offset, data, byteCount);

        InstanceItem item;
        item.mesh = &mesh;
        item.material = &material;
        item.dataOffset = offset;
        item.byteCount = byteCount;
        item.count = instanceCount;
        item.stride = stride;

        m_instancedQueue.push_back(item);
    }

    void AikoRenderer::submit(const Transform& transform, const Mesh& mesh, const MaterialAsset& materialAsset, const MaterialInstance& materialInstance)
    {
        AIKO_FUNCTION_PROFILE
        Material& stagedMaterial = stageMaterial(materialAsset, materialInstance);
        submit(transform, mesh, stagedMaterial);
    }

    void AikoRenderer::submitTransient(const Transform& transform, const Material& material, const MeshAsset& meshAsset, TransientTopology topology)
    {
        AIKO_FUNCTION_PROFILE
        TransientDrawDesc data = {};
        data.mtx = transform.getWorldMatrix();
        data.material = &material;
        data.topology = topology;
        data.geometry = &resolveTransientGeometry(meshAsset, topology);
        m_transientQueue.push_back(std::move(data));
    }

    void AikoRenderer::enqueueCompute(const ComputePass& pass)
    {
        m_computeQueue.push_back(pass);
    }

    void AikoRenderer::requestReadback(const ComputeReadbackRequest& req)
    {
        m_renderer->requestReadback(req);
    }

    bool AikoRenderer::pollReadback(ComputeReadbackResult& out)
    {
        return m_renderer->pollReadback(out);
    }

    void AikoRenderer::drawMeshInstancedGpu(const GpuInstanceDrawDesc& desc)
    {
        m_gpuInstanceDraws.push_back(desc);
    }

    void AikoRenderer::drawBillboards(const GpuBillboardDrawDesc& desc)
    {
        m_gpuBillboardQueue.push_back(desc);
    }

    void AikoRenderer::drawVerticesGpu(const GpuVertexDrawDesc& desc)
    {
        m_gpuVertexDraws.push_back(desc);
    }

    void AikoRenderer::render(const Camera& camera)
    {
        AIKO_FUNCTION_PROFILE
        const renderer::FrameData frameData = buildSceneFrameData(camera);

        m_renderer->bindFrame(COMPUTE_VIEW, frameData);
        executeComputePasses();

        const PreparedScenePass scenePass = buildScenePass();
        submitScenePass(frameData, scenePass, m_renderSurface);

        submitPresentPass(m_renderSurface);

    }

    void AikoRenderer::setDebugTexture(const Texture* texture)
    {
        m_debugTexture = texture;
    }

    const FrameBuffer& AikoRenderer::getTargetTexture() const
    {
        return m_screenFbo.getFrameBuffer();
    }

    void AikoRenderer::waitIdle()
    {
        m_renderer->waitIdle();
    }

    void AikoRenderer::onWindowResize(WindowResizeEvent& event)
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

    renderer::FrameData AikoRenderer::buildSceneFrameData(const Camera& camera) const
    {
        AIKO_FUNCTION_PROFILE
        const renderer::FrameData frameData =
        {
            .view = camera.getViewMatrix(),
            .projection = camera.getProjectionMatrix(),
            .cameraPosition = camera.position,
            .time = static_cast<float>( Time::it().secondSinceStart()),
            .deltaTime = Time::it().getDeltaTime(),
            .ambient = m_ambientLight,
            .lights = m_lights,
        };
        return frameData;
    }

    void AikoRenderer::executeComputePasses()
    {
        AIKO_FUNCTION_PROFILE
        const renderer::PassDescription computePass
        {
            .width = 1,
            .height = 1
        };

        m_renderer->beginPass(COMPUTE_VIEW, computePass, nullptr);

        for (const ComputePass& pass : m_computeQueue)
        {
            m_renderer->execute(COMPUTE_VIEW, pass);
        }

        m_renderer->endPass();
    }

    AikoRenderer::PreparedScenePass AikoRenderer::buildScenePass()
    {
        AIKO_FUNCTION_PROFILE
        PreparedScenePass passData;

        passData.gpuInstances.reserve(m_gpuInstanceDraws.size());
        for (const GpuInstanceDrawDesc& desc : m_gpuInstanceDraws)
        {
            passData.gpuInstances.push_back(&desc);
        }

        passData.gpuBillboards.reserve(m_gpuBillboardQueue.size());
        for (const GpuBillboardDrawDesc& desc : m_gpuBillboardQueue)
        {
            passData.gpuBillboards.push_back(&desc);
        }

        passData.gpuVertices.reserve(m_gpuVertexDraws.size());
        for (const GpuVertexDrawDesc& desc : m_gpuVertexDraws)
        {
            passData.gpuVertices.push_back(&desc);
        }

        passData.opaque.reserve(m_queue.size());
        for (const RenderItem& item : m_queue)
        {
            if (item.mesh == nullptr || item.material == nullptr)
            {
                continue;
            }

            passData.opaque.push_back(PreparedRenderPacket
            {
                .draw =
                {
                    .mesh = item.mesh,
                    .material = item.material,
                    .world = item.transform
                },
                .materialId = item.material->id()
            });
        }

        std::ranges::sort(passData.opaque, [](const PreparedRenderPacket& a, const PreparedRenderPacket& b)
        {
            if (a.materialId != b.materialId)
            {
                return a.materialId < b.materialId;
            }

            if (a.draw.material != b.draw.material)
            {
                return a.draw.material < b.draw.material;
            }

            return a.draw.mesh < b.draw.mesh;
        });

        {
            std::vector<PreparedInstancedPacket> sortedInstanced;
            sortedInstanced.reserve(m_instancedQueue.size());

            size_t totalInstancedBytes = 0;

            for (const InstanceItem& item : m_instancedQueue)
            {
                if (item.mesh == nullptr || item.material == nullptr || item.byteCount == 0)
                {
                    continue;
                }

                AIKO_ASSERT(item.dataOffset + item.byteCount <= m_instanceDataArena.size(), "Instanced data range out of bounds");
                AIKO_ASSERT(item.byteCount == size_t(item.count) * size_t(item.stride), "Instanced byteCount mismatch");

                totalInstancedBytes += item.byteCount;

                sortedInstanced.push_back(PreparedInstancedPacket
                {
                    .draw =
                    {
                        .mesh = item.mesh,
                        .material = item.material,
                        .data = m_instanceDataArena.data() + item.dataOffset,
                        .instanceCount = item.count,
                        .stride = item.stride
                    },
                    .materialId = item.material->id(),
                    .meshId = item.mesh->id(),
                    .mergedDataOffset = 0,
                    .mergedByteCount = 0
                });
            }

            std::ranges::sort(sortedInstanced, [](const PreparedInstancedPacket& a, const PreparedInstancedPacket& b)
            {
                if (a.draw.material != b.draw.material)
                    return a.draw.material < b.draw.material;

                if (a.draw.mesh != b.draw.mesh)
                    return a.draw.mesh < b.draw.mesh;

                return a.draw.stride < b.draw.stride;
            });

            m_mergedInstanceDataArena.clear();
            m_mergedInstanceDataArena.reserve(totalInstancedBytes);

            std::vector<PreparedInstancedPacket> mergedInstanced;
            mergedInstanced.reserve(sortedInstanced.size());

            for (const PreparedInstancedPacket& packet : sortedInstanced)
            {
                const size_t packetByteCount =
                    size_t(packet.draw.instanceCount) * size_t(packet.draw.stride);

                AIKO_ASSERT(packetByteCount > 0, "Merged instanced packet byteCount is zero");

                const auto* srcBytes = static_cast<const uint8_t*>(packet.draw.data);
                AIKO_ASSERT(srcBytes != nullptr, "Prepared instanced packet has null source data");

                if (mergedInstanced.empty())
                {
                    const size_t offset = m_mergedInstanceDataArena.size();
                    m_mergedInstanceDataArena.resize(offset + packetByteCount);
                    memcpy(m_mergedInstanceDataArena.data() + offset, srcBytes, packetByteCount);

                    PreparedInstancedPacket mergedPacket = packet;
                    mergedPacket.mergedDataOffset = offset;
                    mergedPacket.mergedByteCount = packetByteCount;
                    mergedPacket.draw.data = m_mergedInstanceDataArena.data() + offset;

                    mergedInstanced.push_back(std::move(mergedPacket));
                    continue;
                }

                PreparedInstancedPacket& last = mergedInstanced.back();

                const bool compatible =
                    last.draw.mesh == packet.draw.mesh &&
                    last.draw.material == packet.draw.material &&
                    last.draw.stride == packet.draw.stride;

                if (!compatible)
                {
                    const size_t offset = m_mergedInstanceDataArena.size();
                    m_mergedInstanceDataArena.resize(offset + packetByteCount);
                    memcpy(m_mergedInstanceDataArena.data() + offset, srcBytes, packetByteCount);

                    PreparedInstancedPacket mergedPacket = packet;
                    mergedPacket.mergedDataOffset = offset;
                    mergedPacket.mergedByteCount = packetByteCount;
                    mergedPacket.draw.data = m_mergedInstanceDataArena.data() + offset;

                    mergedInstanced.push_back(std::move(mergedPacket));
                    continue;
                }

                const size_t appendOffset = m_mergedInstanceDataArena.size();
                m_mergedInstanceDataArena.resize(appendOffset + packetByteCount);
                memcpy(m_mergedInstanceDataArena.data() + appendOffset, srcBytes, packetByteCount);

                last.draw.instanceCount += packet.draw.instanceCount;
                last.mergedByteCount += packetByteCount;
                last.draw.data = m_mergedInstanceDataArena.data() + last.mergedDataOffset;
            }

            passData.instanced = std::move(mergedInstanced);

        }

        {

            passData.transient.reserve(m_transientQueue.size());

            for (const TransientDrawDesc& item : m_transientQueue)
            {
                const bool hasCachedGeometry =
                    item.geometry != nullptr &&
                    !item.geometry->vertices.empty();

                const bool hasInlineGeometry =
                    !item.vertices.empty();

                if (item.material == nullptr || (!hasCachedGeometry && !hasInlineGeometry))
                {
                    continue;
                }

                passData.transient.push_back(PreparedTransientPacket
                {
                    .item = &item,
                    .materialId = item.material->id()
                });
            }

            std::ranges::sort(passData.transient, [](const PreparedTransientPacket& a, const PreparedTransientPacket& b)
            {
                if (a.materialId != b.materialId)
                {
                    return a.materialId < b.materialId;
                }

                AIKO_ASSERT(a.item != nullptr, "Prepared transient packet a has null item");
                AIKO_ASSERT(b.item != nullptr, "Prepared transient packet b has null item");

                return a.item->material < b.item->material;
            });

        }

        return passData;
    }

    void AikoRenderer::submitScenePass(const renderer::FrameData& frameData, const PreparedScenePass& passData, const ivec2& size)
    {
        AIKO_FUNCTION_PROFILE
        const renderer::PassDescription pass =
        {
            .width = static_cast<u32>(size.x),
            .height = static_cast<u32>(size.y),
            .clearColor = true,
            .clearDepth = true,
            .clear = m_clearColor
        };

        const FrameBuffer& fbo = m_screenFbo.getFrameBuffer();

        std::unordered_set<const Material*> preparedMaterials;

        auto prepareMaterial = [&](const Material* material)
            {
                if (material == nullptr)
                {
                    return;
                }

                if (preparedMaterials.insert(material).second)
                {
                    m_renderer->prepareMaterial(*material);
                }
            };

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

        m_renderer->beginPass(SCENE_VIEW, pass, &fbo);
        m_renderer->bindFrame(SCENE_VIEW, frameData);

        {
            for (const GpuVertexDrawDesc* desc : passData.gpuVertices)
            {
                if (desc != nullptr)
                {
                    m_renderer->submitGpuVertices(SCENE_VIEW, *desc);
                }
            }
        }

        {
            for (const GpuInstanceDrawDesc* desc : passData.gpuInstances)
            {
                if (desc != nullptr)
                {
                    m_renderer->submitGpuInstanced(SCENE_VIEW, *desc);
                }
            }
        }

        {
            for (const GpuBillboardDrawDesc* desc : passData.gpuBillboards)
            {
                if (desc != nullptr)
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

        m_renderer->endPass();
    }

    void AikoRenderer::submitPresentPass(const ivec2& size)
    {
        AIKO_FUNCTION_PROFILE
        const renderer::PassDescription presentPass
        {
            .width = static_cast<u32>(size.x),
            .height = static_cast<u32>(size.y),
            .clearColor = false,
            .clearDepth = false,
        };

        const renderer::FrameData screenFrame =
        {
            .view = mat4(1.0f),
            .projection = mat4(1.0f),
        };

        if (m_debugTexture != nullptr && m_debugTexture->isValid())
        {
            m_renderer->prepareTextureForSampling(*m_debugTexture);
        }
        else
        {
            const TextureBinding* screenBinding = m_screenFbo.getMaterial().textureBinding("u_texture");
            AIKO_ASSERT(screenBinding != nullptr, "ScreenFbo has no texture binding");
            const Texture* screenTexture = screenBinding->runtimeTexture;
            AIKO_ASSERT(screenTexture != nullptr, "ScreenFbo has no runtime texture");
            AIKO_ASSERT( screenTexture->isValid(), "ScreenFbo runtime texture is invalid");
            m_renderer->prepareTextureForSampling(*screenTexture);
        }

        m_renderer->beginPass(SCREEN_VIEW, presentPass, nullptr);
        m_renderer->bindFrame(SCREEN_VIEW, screenFrame);

        if (m_debugTexture != nullptr && m_debugTexture->isValid())
        {
            m_renderer->presentTextureToScreen(SCREEN_VIEW, m_screenFbo, *m_debugTexture);
        }
        else
        {
            m_renderer->presentFrameBufferToScreen(SCREEN_VIEW, m_screenFbo);
        }

        m_imgui.endFrame(size.x, size.y);

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

    const TransientGeometry& AikoRenderer::resolveTransientGeometry(const MeshAsset& meshAsset,
        TransientTopology topology)
    {
        AIKO_FUNCTION_PROFILE

   const TransientCacheKey key
   {
       .meshAsset = &meshAsset,
       .topology = topology
   };

        if (auto it = m_transientGeometryCache.find(key); it != m_transientGeometryCache.end())
        {
            return it->second;
        }

        TransientGeometry geometry{};
        geometry.topology = topology;
        geometry.vertices.reserve(meshAsset.m_vertices.size());

        for (uint32_t i = 0; i < static_cast<uint32_t>(meshAsset.m_vertices.size()); ++i)
        {
            TransientVertex v{};

            v.position = meshAsset.m_vertices[i];

            if (i < meshAsset.m_textCoord.size())
            {
                v.uv = meshAsset.m_textCoord[i];
            }

            if (i < meshAsset.m_normals.size())
            {
                v.normal = meshAsset.m_normals[i];
            }

            if (i < meshAsset.m_colors.size())
            {
                v.color = meshAsset.m_colors[i];
            }
            else
            {
                v.color = WHITE;
            }

            geometry.vertices.push_back(v);
        }

        if (!meshAsset.m_indices.empty())
        {
            geometry.indices = meshAsset.m_indices;
        }

        auto [it, inserted] = m_transientGeometryCache.emplace(key, std::move(geometry));
        AIKO_ASSERT(inserted, "Failed to cache transient geometry");
        return it->second;
    }
}
