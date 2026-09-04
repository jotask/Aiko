#pragma once

#include <aiko_types.h>

#include "interfaces/iframebuffer_impl.h"
#include "platform/vulkan/vulkan_types.h"

namespace aiko::renderer::vulkan
{

    class VulkanFrameBufferImpl : public interfaces::IFrameBufferImpl
    {
    public:
        VulkanFrameBufferImpl();

        virtual ~VulkanFrameBufferImpl() override;

        virtual RenderResourceId id() const override;

        virtual bool isValid() const override;

        // load
        virtual void create(interfaces::ITextureImpl& color, interfaces::ITextureImpl& depth) override;

        virtual void unload() override;

        VkFramebuffer framebuffer() const { return m_framebuffer; }
        VkRenderPass renderPass() const { return m_renderPass; }
        u32 width() const { return m_width; }
        u32 height() const { return m_height; }

    private:

        VkFramebuffer m_framebuffer = VK_NULL_HANDLE;
        VkRenderPass m_renderPass = VK_NULL_HANDLE; // or get/cache from VulkanContext
        u32 m_width = 0;
        u32 m_height = 0;

    };
}
