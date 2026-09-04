#pragma once

#include <cstdint>
#include <vector>
#include <optional>
#include <string>

#include "types/uniform_value.h"

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

    struct VulkanShaderUniformMember
    {
        std::string name;

        UniformType type = UniformType::Unknown;

        uint32_t offset = 0;
        uint32_t size = 0;

        uint32_t matrixStride = 0;
        bool rowMajor = false;
    };

    struct VulkanShaderUniformBlock
    {
        uint32_t set = 0;
        uint32_t binding = 0;
        uint32_t size = 0;

        VkShaderStageFlags stageFlags = 0;

        std::vector<VulkanShaderUniformMember> members;
    };

    struct VulkanShaderReflection
    {
        std::vector<VulkanShaderDescriptorBinding> descriptorBindings;
        std::vector<VulkanShaderPushConstantRange> pushConstantRanges;
        std::optional<VulkanShaderUniformBlock> materialUniformBlock;
        void clear()
        {
            descriptorBindings.clear();
            pushConstantRanges.clear();
            materialUniformBlock = std::nullopt;
        }
    };
}
