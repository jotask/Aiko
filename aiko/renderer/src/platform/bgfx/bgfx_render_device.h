#pragma once

#include "aiko_types.h"
#include "renderer/Irenderdevice.h"

namespace aiko::renderer::bgfx
{

    class BgfxShaderImpl;

    class BgfxRenderDevice final : public IRenderDevice
    {

        #define INVALID_HANDLE {::bgfx::kInvalidHandle};

    public:

        BgfxRenderDevice();
        virtual ~BgfxRenderDevice() override;

        virtual bool init(const renderer::DeviceInitDesc& desc) override;
        virtual void shutdown() override;

        virtual void resize(u32 width, u32 height, bool vsync) override;

        virtual void beginFrame() override;
        virtual void endFrame() override;

        virtual void beginPass(ViewId viewId, const PassDescription& pass, FrameBuffer* frameBuffer = nullptr) override;
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
        virtual void setComputeImage(ViewId viewId, const Texture& texture, ComputeAccess access) override;
        virtual void setComputeBuffer(ViewId viewId, const ComputeBuffer& buffer, ComputeAccess access) override;
        virtual void dispatch( ViewId viewId, const ComputeShader& program, uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ) override;
        virtual void execute(ViewId viewId, const ComputePass& pass) override;

    private:

        u32 m_width;
        u32 m_height;
        bool m_vsync = true;

        FrameData m_frameData;

        BgfxShaderImpl* m_boundShader;

        void bindFrameUniforms();
        void bindMaterialUniforms(const Material& material);

    };
}
