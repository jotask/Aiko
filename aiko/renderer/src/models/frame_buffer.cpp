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
        return backend->isValid() && m_depthAttachment != nullptr && m_depthAttachment->isValid();
    }

    RenderResourceId FrameBuffer::id() const
    {
        return backend->id();
    }

    bool FrameBuffer::hasColorAttachment() const
    {
        return m_colorAttachment != nullptr && m_colorAttachment->isValid();
    }

    bool FrameBuffer::hasDepthAttachment() const
    {
        return m_depthAttachment != nullptr && m_depthAttachment->isValid();
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

        m_colorAttachment = colorTexture.isValid() ? &colorTexture : nullptr;
        m_depthAttachment = &depthTexture;

        backend->create(
            m_colorAttachment ? m_colorAttachment->backend.get() : nullptr,
            m_depthAttachment->backend.get(),
            {}
        );
    }

    void FrameBuffer::createWithAttachments(Texture* color, Texture* depth, TextureSubresource subresource)
    {
        AIKO_ASSERT(depth != nullptr && depth->isValid(), "Framebuffer requires a valid depth attachment");
        AIKO_ASSERT(color == nullptr || color->isValid(), "Invalid framebuffer color attachment");

        const TextureInfo depthInfo = depth->getInfo();

        AIKO_ASSERT(depthInfo.type == TextureType::DepthStencil, "Framebuffer depth attachment must be a depth texture");

        if (color != nullptr)
        {
            const TextureInfo colorInfo = color->getInfo();
            AIKO_ASSERT(colorInfo.type == TextureType::RenderTarget, "Framebuffer color attachment must be a render target");
            AIKO_ASSERT(colorInfo.width == depthInfo.width && colorInfo.height == depthInfo.height, "Framebuffer attachments must have matching dimensions");
        }

        unload();

        m_colorAttachment = color;
        m_depthAttachment = depth;

        backend->create(color ? color->backend.get() : nullptr, depth->backend.get(), subresource);
    }

    void FrameBuffer::unload()
    {
        backend->unload();
        m_colorAttachment = nullptr;
        m_depthAttachment = nullptr;
        colorTexture.unload();
        depthTexture.unload();
    }

    Texture& FrameBuffer::getColorTexture()
    {
        AIKO_ASSERT(m_colorAttachment != nullptr, "Framebuffer has no color attachment");
        return *m_colorAttachment;
    }

    Texture& FrameBuffer::getDepthTexture()
    {
        AIKO_ASSERT(m_depthAttachment != nullptr, "Framebuffer has no depth attachment");
        return *m_depthAttachment;
    }

    const Texture& FrameBuffer::getColorTexture() const
    {
        AIKO_ASSERT(m_colorAttachment != nullptr, "Framebuffer has no color attachment");
        return *m_colorAttachment;
    }

    const Texture& FrameBuffer::getDepthTexture() const
    {
        AIKO_ASSERT(m_depthAttachment != nullptr, "Framebuffer has no depth attachment");
        return *m_depthAttachment;
    }

}
