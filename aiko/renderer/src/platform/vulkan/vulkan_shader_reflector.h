#pragma once

#include <vector>
#include <cstdint>

#include "vulkan_shader_reflection.h"

namespace aiko::renderer::vulkan
{
    void reflectShaderSpirv(const std::vector<uint8_t>& code, VkShaderStageFlagBits stage, VulkanShaderReflection& reflection);
    void validateComputeShaderAbi(const VulkanShaderReflection& reflection);
    void validateModelShaderAbi(const VulkanShaderReflection& reflection);
    void validateModelPushConstants(const VulkanShaderReflection& reflection);
    void validateComputePushConstants(const VulkanShaderReflection& reflection, uint32_t maxSize);
    void validateScreenShaderAbi(const VulkanShaderReflection& reflection);
    void validateScreenPushConstants(const VulkanShaderReflection& reflection);
}
