#pragma once

#include <cstdint>
#include <vector>

#include <volk.h>

namespace aiko::renderer::vulkan
{
    struct VulkanShaderDescriptorBinding
    {
        uint32_t set = 0;
        uint32_t binding = 0;
        VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;
        uint32_t descriptorCount = 0;
        VkShaderStageFlags stageFlags = 0;
    };

    struct VulkanShaderPushConstantRange
    {
        uint32_t offset = 0;
        uint32_t size = 0;
        VkShaderStageFlags stageFlags = 0;
    };

    struct VulkanShaderReflection
    {
        std::vector<VulkanShaderDescriptorBinding> descriptorBindings;
        std::vector<VulkanShaderPushConstantRange> pushConstantRanges;
        void clear()
        {
            descriptorBindings.clear();
            pushConstantRanges.clear();
        }
    };
}
