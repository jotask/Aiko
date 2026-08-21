#include "vulkan_framebuffer_impl.h"

#include "platform/vulkan/vulkan_context.h"

#include <array>

#include <core/file.h>

#include "platform/vulkan/vulkan_types.h"
#include "platform/vulkan/impl/vulkan_texture_impl.h"

namespace aiko::renderer::vulkan
{

    VulkanFrameBufferImpl::VulkanFrameBufferImpl()
    {
    }

    uint VulkanFrameBufferImpl::id() const
    {
        return static_cast<uint>(reinterpret_cast<uintptr_t>(m_framebuffer));
    }

    void VulkanFrameBufferImpl::use()
    {

    }

    void VulkanFrameBufferImpl::unuse()
    {

    }

    bool VulkanFrameBufferImpl::isValid() const
    {
        return m_framebuffer != VK_NULL_HANDLE && m_renderPass != VK_NULL_HANDLE;
    }

    void VulkanFrameBufferImpl::create(Texture& color, Texture& depth)
    {
        unload();

        auto* colorImpl = static_cast<VulkanTextureImpl*>(color.getImpl());
        auto* depthImpl = static_cast<VulkanTextureImpl*>(depth.getImpl());

        AIKO_ASSERT(colorImpl && colorImpl->isValid(), "Invalid Vulkan color texture");
        AIKO_ASSERT(depthImpl && depthImpl->isValid(), "Invalid Vulkan depth texture");

        VulkanContext& ctx = VulkanContext::current();

        const TextureInfo colorInfo = color.getInfo();
        m_width = static_cast<u32>(colorInfo.width);
        m_height = static_cast<u32>(colorInfo.height);

        const VkAttachmentDescription colorAttachment =
        {
            .format = colorImpl->vkFormat(),
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        };

        const VkAttachmentDescription depthAttachment =
        {
            .format = depthImpl->vkFormat(),
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        };

        const VkAttachmentReference colorRef =
        {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        };

        const VkAttachmentReference depthRef =
        {
            .attachment = 1,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        };

        const VkSubpassDescription subpass =
        {
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colorRef,
            .pDepthStencilAttachment = &depthRef,
        };

        const std::array<VkAttachmentDescription, 2> attachments =
        {
            colorAttachment,
            depthAttachment
        };

        const std::array<VkSubpassDependency, 2> dependencies =
        {
            VkSubpassDependency
            {
                .srcSubpass = VK_SUBPASS_EXTERNAL,
                .dstSubpass = 0,
                .srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                .srcAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
            },
            VkSubpassDependency
            {
                .srcSubpass = 0,
                .dstSubpass = VK_SUBPASS_EXTERNAL,
                .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                .dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                .dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
            }
        };

        const VkRenderPassCreateInfo renderPassInfo =
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .attachmentCount = static_cast<uint32_t>(attachments.size()),
            .pAttachments = attachments.data(),
            .subpassCount = 1,
            .pSubpasses = &subpass,
            .dependencyCount = static_cast<uint32_t>(dependencies.size()),
            .pDependencies = dependencies.data(),
        };

        VkResult result = vkCreateRenderPass(ctx.device(), &renderPassInfo, nullptr, &m_renderPass);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan framebuffer render pass");

        const std::array<VkImageView, 2> imageViews =
        {
            colorImpl->imageView(),
            depthImpl->imageView()
        };

        const VkFramebufferCreateInfo framebufferInfo =
        {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = m_renderPass,
            .attachmentCount = static_cast<uint32_t>(imageViews.size()),
            .pAttachments = imageViews.data(),
            .width = m_width,
            .height = m_height,
            .layers = 1,
        };

        result = vkCreateFramebuffer(ctx.device(), &framebufferInfo, nullptr, &m_framebuffer);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan framebuffer");
    }

    void VulkanFrameBufferImpl::unload()
    {
        if (m_framebuffer == VK_NULL_HANDLE && m_renderPass == VK_NULL_HANDLE)
        {
            return;
        }

        VkDevice device = VulkanContext::current().device();

        if (m_framebuffer != VK_NULL_HANDLE) vkDestroyFramebuffer(device, m_framebuffer, nullptr);
        if (m_renderPass != VK_NULL_HANDLE) vkDestroyRenderPass(device, m_renderPass, nullptr);

        m_framebuffer = VK_NULL_HANDLE;
        m_renderPass = VK_NULL_HANDLE;
        m_width = 0;
        m_height = 0;
    }

}
