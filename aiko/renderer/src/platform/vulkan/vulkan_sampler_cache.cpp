#include "vulkan_sampler_cache.h"

#include "vulkan_context.h"

namespace aiko::renderer::vulkan
{
    namespace
    {
        VkFilter toVulkanFilter(TextureFilter filter)
        {
            switch (filter)
            {
                case TextureFilter::Nearest: return VK_FILTER_NEAREST;
                case TextureFilter::Linear: return VK_FILTER_LINEAR;
            }
            AIKO_ASSERT(false, "Unsupported texture filter");
            return VK_FILTER_LINEAR;
        }

        VkSamplerMipmapMode toVulkanMipFilter(TextureMipFilter filter)
        {
            switch (filter)
            {
                case TextureMipFilter::None:
                case TextureMipFilter::Nearest:
                    return VK_SAMPLER_MIPMAP_MODE_NEAREST;

                case TextureMipFilter::Linear:
                    return VK_SAMPLER_MIPMAP_MODE_LINEAR;
            }

            AIKO_ASSERT(false, "Unsupported texture mip filter");
            return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        }

        VkSamplerAddressMode toVulkanWrapMode(TextureWrapMode mode)
        {
            switch (mode)
            {
                case TextureWrapMode::Repeat:
                    return VK_SAMPLER_ADDRESS_MODE_REPEAT;

                case TextureWrapMode::Clamp:
                    return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

                case TextureWrapMode::Mirror:
                    return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
            }

            AIKO_ASSERT(false, "Unsupported texture wrap mode");
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        }
    }

    VulkanSamplerCache::VulkanSamplerCache(VulkanContext& context)
        : m_context(context)
    {
    }

    VkSampler VulkanSamplerCache::getOrCreate(const SamplerState& state)
    {
        const auto it = m_samplers.find(state);

        if (it != m_samplers.end())
        {
            return it->second;
        }

        const VkSamplerCreateInfo info =
        {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .magFilter = toVulkanFilter(state.magFilter),
            .minFilter = toVulkanFilter(state.minFilter),
            .mipmapMode = toVulkanMipFilter(state.mipFilter),
            .addressModeU = toVulkanWrapMode(state.wrapU),
            .addressModeV = toVulkanWrapMode(state.wrapV),
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .mipLodBias = 0.0f,
            .anisotropyEnable = VK_FALSE,
            .maxAnisotropy = 1.0f,
            .compareEnable = VK_FALSE,
            .compareOp = VK_COMPARE_OP_ALWAYS,
            .minLod = 0.0f,
            .maxLod = state.mipFilter == TextureMipFilter::None ? 0.0f : VK_LOD_CLAMP_NONE,
            .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
            .unnormalizedCoordinates = VK_FALSE,
        };

        VkSampler sampler = VK_NULL_HANDLE;

        const VkResult result = vkCreateSampler(m_context.device(), &info, nullptr, &sampler);

        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan sampler");

        m_samplers.emplace(state, sampler);

        return sampler;
    }

    void VulkanSamplerCache::destroy()
    {
        VkDevice device = m_context.device();

        for (const auto& [state, sampler] : m_samplers)
        {
            AIKO_UNUSED(state);

            if (sampler != VK_NULL_HANDLE)
            {
                vkDestroySampler(device, sampler, nullptr);
            }
        }

        m_samplers.clear();
    }
}
