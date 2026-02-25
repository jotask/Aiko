#include "frame_buffer.h"

#include "render_factory.h"

namespace aiko
{

    FrameBuffer::FrameBuffer()
        : backend(renderer::RendererFactory::createFrameBufferImpl())
    {

    }

    void FrameBuffer::use()
    {
        backend->use();
    }

    void FrameBuffer::unuse()
    {
        backend->unuse();
    }

    bool FrameBuffer::isValid() const
    {
        return backend->isValid() && colorTexture.isValid() && depthTexture.isValid();
    }

    uint FrameBuffer::id() const
    {
        return backend->id();
    }

    void FrameBuffer::create(int width, int height)
    {
        colorTexture.create(
            {
                .type = texture::TextureType::RenderTarget,
                .format = texture::TextureFormat::RGBA8,
                .width = width,
                .height = height,
                .mipmaps = false,
            });
        depthTexture.create(
            {
                .type = texture::TextureType::DepthStencil,
                .format = texture::TextureFormat::D24S8,
                .width = width,
                .height = height,
                .mipmaps = false,
            });
        backend->create(colorTexture, depthTexture);
    }

    void FrameBuffer::unload()
    {
        backend->unload();
    }

}
