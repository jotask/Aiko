#pragma once

#include <deque>
#include <optional>

#include "aiko_types.h"
#include "renderer/Irenderdevice.h"

namespace aiko::renderer::bgfx
{

    class BgfxShaderImpl;

    class BgfxRenderDevice final : public IRenderDevice
    {
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
        virtual void execute(ViewId viewId, const ComputePass& pass) override;
        virtual void requestReadback( const ComputeReadbackRequest& request) override;
        virtual bool pollReadback(ComputeReadbackResult& result) override;

    private:

        static constexpr ViewId READBACK_VIEW = 250;
        static constexpr ViewId READBACK_BLIT_VIEW = 251;

        // --- Compute readback (BGFX internal) ---
        struct PendingReadback
        {
            ReadbackId id = 0;
            const ComputeBuffer* source = nullptr;
            uint32_t byteSize = 0;

            ::bgfx::TextureHandle computeTex = { ::bgfx::kInvalidHandle }; // compute-write
            ::bgfx::TextureHandle readTex    = { ::bgfx::kInvalidHandle }; // read-back

            uint32_t framesSinceRequest = 0;

            enum class Stage : uint8_t
            {
                Idle = 0,
                SubmittedGPUWork,
                ReadIssued,
                Done
            };

            Stage stage = Stage::Idle;
            uint32_t framesSinceSubmit = 0;

            std::vector<uint8_t> cpu;
        };

        void dispatchPendingReadbackCopy();
        void startReadbackInternal(const ComputeReadbackRequest& r);
        void cleanupReadback(PendingReadback& rb);

        std::deque<ComputeReadbackRequest> m_readbackQueue;
        std::deque<ComputeReadbackResult>  m_completedReadbacks;
        std::optional<PendingReadback>     m_activeReadback;

        ReadbackId m_nextReadbackId = 1;

        // Hidden compute shader that copies buffer -> texture
        ComputeShader m_csReadbackCopy;

        u32 m_width;
        u32 m_height;
        bool m_vsync = true;

        FrameData m_frameData;

        BgfxShaderImpl* m_boundShader;

        void bindFrameUniforms();
        void bindMaterialUniforms(const Material& material);

    };
}
