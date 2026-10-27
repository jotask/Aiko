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
        return backend->isValid() && depthTexture.isValid();
    }

    RenderResourceId FrameBuffer::id() const
    {
        return backend->id();
    }


    void FrameBuffer::create(const FrameBufferDesc& desc)
    {

        AIKO_ASSERT(desc.width > 0 && desc.height > 0, "Framebuffer dimensions must be positive");
        AIKO_ASSERT(desc.colorFormat == TextureFormat::None || desc.colorFormat == TextureFormat::RGBA8 || desc.colorFormat == TextureFormat::RGBA16F, "Unsupported framebuffer color format");
        AIKO_ASSERT(desc.depthFormat == TextureFormat::INVALID || desc.depthFormat == TextureFormat::D32F || desc.depthFormat == TextureFormat::D24S8, "Unsupported framebuffer depth format");

        unload();

        if (desc.colorFormat != TextureFormat::None)
        {
            colorTexture.create({
                .type = TextureType::RenderTarget,
                .format = desc.colorFormat,
                .width = desc.width,
                .height = desc.height,
                .mipmaps = 1,
                .computeWrite = desc.colorFormat == TextureFormat::RGBA8,
            });

            if (!colorTexture.isValid())
            {
                unload();
                return;
            }
        }

        const TextureFormat depthFormat = desc.depthFormat == TextureFormat::INVALID ? backend->preferredDepthFormat() : desc.depthFormat;

        depthTexture.create({
            .type = TextureType::DepthStencil,
            .format = depthFormat,
            .width = desc.width,
            .height = desc.height,
            .mipmaps = 1,
        });

        if (!depthTexture.isValid())
        {
            unload();
            return;
        }

        backend->create(colorTexture.isValid() ? colorTexture.backend.get() : nullptr, depthTexture.backend.get());
    }


    void FrameBuffer::unload()
    {
        backend->unload();
        colorTexture.unload();
        depthTexture.unload();
    }

}
