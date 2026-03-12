 #pragma once

#include <aiko_types.h>
#include <math/math.h>

#include "texture.h"
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
        FrameBuffer(const FrameBuffer&) = delete;
        FrameBuffer& operator=(const FrameBuffer&) = delete;

        // Move
        FrameBuffer(FrameBuffer&&) noexcept = default;
        FrameBuffer& operator=(FrameBuffer&&) noexcept = default;

        FrameBuffer();
        virtual ~FrameBuffer() = default;

        void* getImpl() const { return backend.get(); }

        void use();
        void unuse();
        bool isValid() const;
        uint id() const;

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
