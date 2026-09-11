 #pragma once

#include <aiko_types.h>
#include "types/render_types.h"

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

        // load
        void create(int width, int height);
        void unload();

        Texture& getColorTexture() { return colorTexture; }
        Texture& getDepthTexture() { return depthTexture; }
        const Texture& getColorTexture() const { return colorTexture; }
        const Texture& getDepthTexture() const { return depthTexture; }

    private:

        AikoPtr<interfaces::IFrameBufferImpl> backend;

        Texture colorTexture;
        Texture depthTexture;

    };

}
