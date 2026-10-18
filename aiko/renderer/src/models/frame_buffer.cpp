#include "models/frame_buffer.h"

#include "render_factory.h"
#include "interfaces/iframebuffer_impl.h"

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
        create(width, height, TextureFormat::RGBA8);
    }

    void FrameBuffer::create(int width, int height, TextureFormat colorFormat)
    {
        AIKO_ASSERT(width > 0 && height > 0, "Framebuffer size must be greater than zero");
        AIKO_ASSERT(colorFormat == TextureFormat::RGBA8 || colorFormat == TextureFormat::RGBA16F, "Unsupported framebuffer color format");

        unload();

        colorTexture.create({
            .type = TextureType::RenderTarget,
            .format = colorFormat,
            .width = width,
            .height = height,
            .mipmaps = 1,
            .computeWrite = colorFormat == TextureFormat::RGBA8,
        });

        if (!colorTexture.isValid())
        {
            unload();
            return;
        }

        depthTexture.create({
            .type = TextureType::DepthStencil,
            .format = backend->preferredDepthFormat(),
            .width = width,
            .height = height,
            .mipmaps = 1,
        });

        if (!depthTexture.isValid())
        {
            unload();
            return;
        }

        backend->create(*colorTexture.backend, *depthTexture.backend);
    }

    void FrameBuffer::unload()
    {
        backend->unload();
        colorTexture.unload();
        depthTexture.unload();
    }

}
