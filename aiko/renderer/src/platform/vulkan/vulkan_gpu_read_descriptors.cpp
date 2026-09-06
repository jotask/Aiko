#include "vulkan_gpu_read_descriptors.h"

#include <array>

#include "vulkan_context.h"
#include "vulkan_descriptor_abi.h"

namespace aiko::renderer::vulkan
{
    VulkanGpuReadDescriptors::VulkanGpuReadDescriptors(VulkanContext& context, uint32_t frameCount)
        : m_context(context)
        , m_pools(frameCount, VK_NULL_HANDLE)
    {
        AIKO_ASSERT(frameCount > 0, "Vulkan GPU-read descriptors require at least one frame");
    }

    void VulkanGpuReadDescriptors::create()
    {
        AIKO_ASSERT(m_layout == VK_NULL_HANDLE, "Vulkan GPU-read descriptors already created");

        std::array<VkDescriptorSetLayoutBinding, abi::MaxGpuReadBindings> bindings{};

        for (uint32_t i = 0; i < abi::MaxGpuReadBindings; ++i)
        {
            bindings[i] =
            {
                .binding = i,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr,
            };
        }

        const VkDescriptorSetLayoutCreateInfo layoutInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = static_cast<uint32_t>(bindings.size()),
            .pBindings = bindings.data(),
        };

        const VkResult layoutResult = vkCreateDescriptorSetLayout(m_context.device(), &layoutInfo, nullptr, &m_layout);

        AIKO_ASSERT(layoutResult == VK_SUCCESS, "Failed to create GPU-read descriptor layout");

        for (VkDescriptorPool& pool : m_pools)
        {
            const VkDescriptorPoolSize poolSize =
            {
                .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = abi::MaxGpuReadBindings * MaxSetsPerFrame,
            };

            const VkDescriptorPoolCreateInfo poolInfo =
            {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                .maxSets = MaxSetsPerFrame,
                .poolSizeCount = 1,
                .pPoolSizes = &poolSize,
            };

            const VkResult poolResult = vkCreateDescriptorPool(m_context.device(), &poolInfo, nullptr, &pool);
            AIKO_ASSERT(poolResult == VK_SUCCESS, "Failed to create GPU-read descriptor pool");
        }
    }

    void VulkanGpuReadDescriptors::destroy()
    {
        VkDevice device = m_context.device();

        for (VkDescriptorPool& pool : m_pools)
        {
            if (pool != VK_NULL_HANDLE)
            {
                vkDestroyDescriptorPool(device, pool, nullptr);
                pool = VK_NULL_HANDLE;
            }
        }

        if (m_layout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(device, m_layout, nullptr);
            m_layout = VK_NULL_HANDLE;
        }
    }

    void VulkanGpuReadDescriptors::resetFrame(uint32_t frameIndex)
    {
        AIKO_ASSERT(frameIndex < m_pools.size(), "Invalid GPU-read descriptor frame index");
        AIKO_ASSERT(m_pools[frameIndex] != VK_NULL_HANDLE, "GPU-read descriptor pool is invalid");
        const VkResult result = vkResetDescriptorPool(m_context.device(), m_pools[frameIndex], 0);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to reset GPU-read descriptor pool");
    }

    VkDescriptorSet VulkanGpuReadDescriptors::allocate(uint32_t frameIndex)
    {
        AIKO_ASSERT(frameIndex < m_pools.size(), "Invalid GPU-read descriptor frame index");
        AIKO_ASSERT(m_layout != VK_NULL_HANDLE, "GPU-read descriptor layout is invalid");

        const VkDescriptorSetAllocateInfo info =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = m_pools[frameIndex],
            .descriptorSetCount = 1,
            .pSetLayouts = &m_layout,
        };

        VkDescriptorSet set = VK_NULL_HANDLE;

        const VkResult result = vkAllocateDescriptorSets(m_context.device(), &info, &set);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to allocate GPU-read descriptor set");

        return set;
    }
}
