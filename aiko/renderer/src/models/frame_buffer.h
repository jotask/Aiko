 #pragma once

#include <aiko_types.h>
#include <math/math.h>

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

        void* getImpl() const { return backend.get(); }

        void use();
        void unuse();
        bool isValid();
        uint id() const;

        // load
        void create(int width, int height);
        void unload();

        // getter
        ivec2 getSize() const;

    private:

        AikoPtr<interfaces::IFrameBufferImpl> backend;

        uint framebuffer;
        Texture texture;
        Texture depth;

    };

}
