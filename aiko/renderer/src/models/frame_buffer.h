 #pragma once

#include <aiko_types.h>
#include "types/render_types.h"
#include <metadata/frame_buffer_meta.h>

#include "texture.h"

namespace aiko::interfaces
{
    class IFrameBufferImpl;
}

 namespace aiko
{
    namespace renderer
    {
        class RendererFactory;
        class IRenderDevice;
    }

    class FrameBuffer
    {
        friend class renderer::IRenderDevice;
        friend class renderer::RendererFactory;
    public:

        // Copy
        FrameBuffer(const FrameBuffer&) = delete;
        FrameBuffer& operator=(const FrameBuffer&) = delete;

        // Move
        FrameBuffer(FrameBuffer&&) noexcept = delete;
        FrameBuffer& operator=(FrameBuffer&&) noexcept = delete;

        FrameBuffer();
        virtual ~FrameBuffer();

        bool isValid() const;
        RenderResourceId id() const;

        bool hasColorAttachment() const;
        bool hasDepthAttachment() const;

        // load
        void create(const FrameBufferDesc& desc);
        void createWithAttachments(Texture* color, Texture* depth, TextureSubresource subresource = {});
        void unload();

        Texture& getColorTexture();
        Texture& getDepthTexture();
        const Texture& getColorTexture() const;
        const Texture& getDepthTexture() const;

    private:

        AikoPtr<interfaces::IFrameBufferImpl> backend;

        Texture colorTexture;
        Texture depthTexture;

        Texture* m_colorAttachment = nullptr;
        Texture* m_depthAttachment = nullptr;

    };

}
