#include "vulkan_framebuffer_impl.h"

#include "platform/vulkan/vulkan_context.h"

#include <array>
#include <vector>

#include <core/file.h>

#include "platform/vulkan/vulkan_types.h"
#include "platform/vulkan/impl/vulkan_texture_impl.h"

namespace aiko::renderer::vulkan
{

    VulkanFrameBufferImpl::VulkanFrameBufferImpl()
    {
    }

    VulkanFrameBufferImpl::~VulkanFrameBufferImpl()
    {
        AIKO_ASSERT(m_framebuffer == VK_NULL_HANDLE && m_renderPass == VK_NULL_HANDLE, "VulkanFrameBufferImpl destroyed without unload()" );
    }

    RenderResourceId VulkanFrameBufferImpl::id() const
    {
        return static_cast<uint>(reinterpret_cast<uintptr_t>(m_framebuffer));
    }

    bool VulkanFrameBufferImpl::isValid() const
    {
        return m_framebuffer != VK_NULL_HANDLE && m_renderPass != VK_NULL_HANDLE;
    }

    TextureFormat VulkanFrameBufferImpl::preferredDepthFormat() const
    {
        VulkanContext& ctx = VulkanContext::current();

        VkFormatProperties properties{};

        const VkFormatFeatureFlags requiredFeatures = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;

        vkGetPhysicalDeviceFormatProperties(ctx.physicalDevice(), VK_FORMAT_D32_SFLOAT, &properties);

        if ((properties.optimalTilingFeatures & requiredFeatures) == requiredFeatures)
        {
            return TextureFormat::D32F;
        }

        properties = {};

        vkGetPhysicalDeviceFormatProperties(ctx.physicalDevice(), VK_FORMAT_D24_UNORM_S8_UINT, &properties);

        AIKO_ASSERT((properties.optimalTilingFeatures & requiredFeatures) == requiredFeatures, "No supported sampleable Aiko depth format");

        return TextureFormat::D24S8;
    }

    void VulkanFrameBufferImpl::create(interfaces::ITextureImpl* color, interfaces::ITextureImpl* depth)
    {
        unload();

        AIKO_ASSERT(depth != nullptr, "Framebuffer requires a depth attachment");
        AIKO_ASSERT(depth->isValid(), "Invalid Vulkan depth texture");

        const bool hasColor = color != nullptr;

        if (hasColor)
        {
            AIKO_ASSERT(color->isValid(), "Invalid Vulkan color texture");
        }

        auto* colorImpl = hasColor ? static_cast<VulkanTextureImpl*>(color) : nullptr;

        auto* depthImpl = static_cast<VulkanTextureImpl*>(depth);

        VulkanContext& ctx = VulkanContext::current();

        const TextureInfo depthInfo = depth->getInfo();

        m_width = static_cast<u32>(depthInfo.width);
        m_height = static_cast<u32>(depthInfo.height);

        if (hasColor)
        {
            const TextureInfo colorInfo = color->getInfo();
            AIKO_ASSERT(colorInfo.width == depthInfo.width && colorInfo.height == depthInfo.height, "Framebuffer attachments must have matching dimensions");
        }

        const VkAttachmentDescription colorAttachment =
        {
            .format = hasColor ? colorImpl->format() : VK_FORMAT_UNDEFINED,
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
            .format = depthImpl->format(),
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
        };

        const VkAttachmentReference colorRef{
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        };

        const VkAttachmentReference depthRef{
            .attachment = hasColor ? 1u : 0u,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        };

        const VkSubpassDescription subpass{
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .colorAttachmentCount = hasColor ? 1u : 0u,
            .pColorAttachments = hasColor ? &colorRef : nullptr,
            .pDepthStencilAttachment = &depthRef,
        };

        std::vector<VkAttachmentDescription> attachments;
        std::vector<VkImageView> imageViews;

        if (hasColor)
        {
            attachments.push_back(colorAttachment);
            imageViews.push_back(colorImpl->imageView());
        }

        attachments.push_back(depthAttachment);
        imageViews.push_back(depthImpl->imageView());

        const VkPipelineStageFlags attachmentStages =
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
            VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT |
            (hasColor ? VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT : 0);

        const VkAccessFlags attachmentWrites =
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
            (hasColor ? VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT : 0);

        const std::array<VkSubpassDependency, 2> dependencies =
        {
            VkSubpassDependency
            {
                .srcSubpass = VK_SUBPASS_EXTERNAL,
                .dstSubpass = 0,
                .srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                .dstStageMask = attachmentStages,
                .srcAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                .dstAccessMask = attachmentWrites,
                .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
            },
            VkSubpassDependency
            {
                .srcSubpass = 0,
                .dstSubpass = VK_SUBPASS_EXTERNAL,
                .srcStageMask = attachmentStages,
                .dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                .srcAccessMask = attachmentWrites,
                .dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                .dependencyFlags = 0,
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

        const VkFramebuffer framebuffer = m_framebuffer;
        const VkRenderPass renderPass = m_renderPass;

        m_framebuffer = VK_NULL_HANDLE;
        m_renderPass = VK_NULL_HANDLE;
        m_width = 0;
        m_height = 0;

        VulkanContext::current().retireFrameBuffer(framebuffer, renderPass);

    }

}
