#include "models/frame_buffer.h"

#include "render_factory.h"

namespace aiko
{

    FrameBuffer::FrameBuffer()
        : backend(renderer::RendererFactory::createFrameBufferImpl())
    {

    }

    FrameBuffer::~FrameBuffer()
    {
        unload();
    }

    bool FrameBuffer::isValid() const
    {
        return backend->isValid() && colorTexture.isValid() && depthTexture.isValid();
    }

    RenderResourceId FrameBuffer::id() const
    {
        return backend->id();
    }

    void FrameBuffer::create(int width, int height)
    {
        AIKO_ASSERT(width > 0 && height > 0, "Framebuffer size must be greater than zero");

        unload();

        colorTexture.create(
            {
                .type = TextureType::RenderTarget,
                .format = TextureFormat::RGBA8,
                .width = width,
                .height = height,
                .mipmaps = false,
                .computeWrite = true,
            });
        depthTexture.create(
            {
                .type = TextureType::DepthStencil,
                .format = TextureFormat::D24S8,
                .width = width,
                .height = height,
                .mipmaps = false,
            });
        backend->create(*colorTexture.backend, *depthTexture.backend);
    }

    void FrameBuffer::unload()
    {
        backend->unload();
        colorTexture.unload();
        depthTexture.unload();
    }

}
