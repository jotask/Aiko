 #pragma once

#include <aiko_types.h>

#include "models/texture.h"
#include "interfaces/iframebuffer_impl.h"

 namespace aiko
{
    namespace renderer
    {
        class RendererFactory;
    }

    class FrameBuffer
    {
    public:

        friend class renderer::RendererFactory;

        // Copy
        FrameBuffer(const FrameBuffer&) = default;
        FrameBuffer& operator=(const FrameBuffer&) = default;

        // Move
        FrameBuffer(FrameBuffer&&) noexcept = default;
        FrameBuffer& operator=(FrameBuffer&&) noexcept = default;

        FrameBuffer();
        virtual ~FrameBuffer() = default;

        virtual void use() = 0;
        virtual void unuse() = 0;
        virtual bool isValid() = 0;
        virtual uint id() const = 0;

        // load
        virtual void create(int width, int height) = 0;
        virtual void unload() = 0;

    private:

        AikoPtr<interfaces::IFrameBufferImpl> backend;

        uint framebuffer;
        Texture texture;
        Texture depth;

    };

}
