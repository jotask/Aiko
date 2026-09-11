#pragma once

#include <unordered_map>

#include <volk.h>

#include "vulkan_render_types.h"

namespace aiko::renderer::vulkan
{
    class VulkanContext;

    class VulkanSamplerCache final
    {
    public:
        VulkanSamplerCache(const VulkanSamplerCache&) = delete;
        VulkanSamplerCache& operator=(const VulkanSamplerCache&) = delete;

        VulkanSamplerCache(VulkanSamplerCache&&) = delete;
        VulkanSamplerCache& operator=(VulkanSamplerCache&&) = delete;

        explicit VulkanSamplerCache(VulkanContext& context);

        VkSampler getOrCreate(const SamplerState& state);

        void destroy();

    private:
        VulkanContext& m_context;

        std::unordered_map<SamplerState, VkSampler, SamplerStateHash> m_samplers;
    };
}
