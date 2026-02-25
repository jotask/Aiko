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

        virtual void beginPass(ViewId viewId, const PassDescription& pass, void* nativeFrameHandler = nullptr) override;
        virtual void endPass() override;

        virtual void present() override;

        virtual void setViewTransform(ViewId viewId, const mat4& view, const mat4& projection) override;

        virtual void renderMesh(ViewId viewId, const mat4 world, const Mesh& mesh, const Material& material) override;

        virtual void bindMaterial(const Material& material) override;
        virtual void drawMesh(ViewId viewId, const mat4& world, const Mesh& mesh, const Material& material) override;

    private:

        u32 m_width;
        u32 m_height;
        bool m_vsync = true;

        BgfxShaderImpl* m_boundShader;

    };
}
