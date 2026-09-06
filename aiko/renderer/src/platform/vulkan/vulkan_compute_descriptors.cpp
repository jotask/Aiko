#include "vulkan_compute_descriptors.h"

#include <array>

#include "vulkan_context.h"
#include "vulkan_descriptor_abi.h"

namespace aiko::renderer::vulkan
{
    VulkanComputeDescriptors::VulkanComputeDescriptors(VulkanContext& context, uint32_t frameCount)
        : m_context(context)
        , m_pools(frameCount, VK_NULL_HANDLE)
    {
        AIKO_ASSERT(frameCount > 0, "Vulkan compute descriptors require at least one frame");
    }

    void VulkanComputeDescriptors::create()
    {
        AIKO_ASSERT(m_layout == VK_NULL_HANDLE, "Vulkan compute descriptors already created");

        std::array<VkDescriptorSetLayoutBinding, abi::MaxComputeBufferBindings + abi::MaxComputeImageBindings + 1> bindings{};

        for (uint32_t i = 0; i < abi::MaxComputeBufferBindings; ++i)
        {
            bindings[i] =
            {
                .binding = i,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
                .pImmutableSamplers = nullptr,
            };
        }

        for (uint32_t i = 0; i < abi::MaxComputeImageBindings; ++i)
        {
            const uint32_t bindingIndex = abi::ComputeImageBindingBase + i;

            bindings[bindingIndex] =
            {
                .binding = bindingIndex,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
                .pImmutableSamplers = nullptr,
            };
        }

        bindings[abi::ComputeFrameBinding] =
        {
            .binding = abi::ComputeFrameBinding,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = nullptr,
        };

        const VkDescriptorSetLayoutCreateInfo layoutInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = static_cast<uint32_t>(bindings.size()),
            .pBindings = bindings.data(),
        };

        const VkResult layoutResult = vkCreateDescriptorSetLayout(m_context.device(), &layoutInfo, nullptr, &m_layout);
        AIKO_ASSERT(layoutResult == VK_SUCCESS, "Failed to create compute descriptor set layout");

        for (VkDescriptorPool& pool : m_pools)
        {
            const std::array<VkDescriptorPoolSize, 3> poolSizes =
            {
                VkDescriptorPoolSize
                {
                    .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    .descriptorCount = abi::MaxComputeBufferBindings * MaxDispatchesPerFrame,
                },

                VkDescriptorPoolSize
                {
                    .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                    .descriptorCount = abi::MaxComputeImageBindings * MaxDispatchesPerFrame,
                },

                VkDescriptorPoolSize
                {
                    .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    .descriptorCount = MaxDispatchesPerFrame,
                }
            };

            const VkDescriptorPoolCreateInfo poolInfo =
            {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                .maxSets = MaxDispatchesPerFrame,
                .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
                .pPoolSizes = poolSizes.data(),
            };

            const VkResult result = vkCreateDescriptorPool(m_context.device(), &poolInfo, nullptr, &pool);
            AIKO_ASSERT(result == VK_SUCCESS, "Failed to create compute descriptor pool");

        }
    }

    void VulkanComputeDescriptors::destroy()
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

    void VulkanComputeDescriptors::resetFrame(uint32_t frameIndex)
    {
        AIKO_ASSERT(frameIndex < m_pools.size(), "Invalid compute descriptor frame index");
        AIKO_ASSERT(m_pools[frameIndex] != VK_NULL_HANDLE, "Compute descriptor pool is invalid");
        const VkResult result = vkResetDescriptorPool(m_context.device(), m_pools[frameIndex], 0);
        AIKO_ASSERT(result == VK_SUCCESS,"Failed to reset compute descriptor pool");
    }

    VkDescriptorSet VulkanComputeDescriptors::allocate(uint32_t frameIndex)
    {
        AIKO_ASSERT(frameIndex < m_pools.size(), "Invalid compute descriptor frame index");
        AIKO_ASSERT(m_pools[frameIndex] != VK_NULL_HANDLE, "Compute descriptor pool is invalid");
        AIKO_ASSERT(m_layout != VK_NULL_HANDLE, "Compute descriptor layout is invalid");

        const VkDescriptorSetAllocateInfo info =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = m_pools[frameIndex],
            .descriptorSetCount = 1,
            .pSetLayouts = &m_layout,
        };

        VkDescriptorSet set = VK_NULL_HANDLE;

        const VkResult result = vkAllocateDescriptorSets(m_context.device(), &info, &set);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to allocate compute descriptor set");

        return set;
    }
}
