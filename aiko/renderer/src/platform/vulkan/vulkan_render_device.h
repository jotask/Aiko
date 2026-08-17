#pragma once

#include <deque>
#include <optional>
#include <unordered_set>

#include "aiko_types.h"
#include "vulkan_types.h"
#include "core/utils.h"
#include "renderer/Irenderdevice.h"

namespace aiko::renderer::vulkan
{
    class VulkanTextureImpl;

    class VulkanShaderImpl;

    class VulkanRenderDevice final : public IRenderDevice
    {
    public:

        VulkanRenderDevice(RenderResourceManager*);
        virtual ~VulkanRenderDevice() override;

        virtual bool init(const renderer::DeviceInitDesc& desc) override;
        virtual void shutdown() override;

        virtual void resize(u32 width, u32 height, bool vsync) override;

        virtual void beginFrame() override;
        virtual void endFrame() override;

        virtual void beginPass(ViewId viewId, const PassDescription& pass, const FrameBuffer* frameBuffer = nullptr) override;
        virtual void endPass() override;

        virtual void present() override;

        virtual void renderMesh(ViewId viewId, const mat4 world, const Mesh& mesh, const Material& material) override;

        virtual void bindMaterial(const Material& material) override;
        virtual void drawMesh(ViewId viewId, const mat4& world, const Mesh& mesh, const Material& material) override;

        virtual void presentFrameBufferToScreen(ViewId viewId, const ScreenFbo& fb) override;
        virtual void presentTextureToScreen(ViewId viewId, const ScreenFbo& screen, const Texture& texture) override;

        virtual void drawMeshInstanced(ViewId viewId, const Mesh& mesh, const Material& material, const void* data, u32 instanceCount, u32 instanceStrideBytes) override;

        virtual void bindFrame(ViewId viewId, const FrameData& u) override;

        // Compute Shader
        virtual void execute(ViewId viewId, const ComputePass& pass) override;
        virtual void requestReadback(const ComputeReadbackRequest& request) override;
        virtual bool pollReadback(ComputeReadbackResult& result) override;

        virtual void drawMeshInstancedGpu(ViewId viewId, const GpuInstanceDrawDesc& desc) override;
        virtual void drawBillboards(ViewId viewId, const GpuBillboardDrawDesc& desc) override;

        virtual void drawTransient(ViewId viewId, const TransientDrawDesc& desc) override;

    private:

    };
}
