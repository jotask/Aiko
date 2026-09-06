#include "vulkan_frame_resources.h"

#include <array>
#include <cstring>

#include "vulkan_context.h"
#include "vulkan_descriptor_abi.h"

namespace aiko::renderer::vulkan
{
    VulkanFrameResources::VulkanFrameResources(
        VulkanContext& context,
        uint32_t frameCount
    )
        : m_context(context)
        , m_pools(frameCount, VK_NULL_HANDLE)
        , m_bindings(frameCount)
    {
        AIKO_ASSERT(frameCount > 0, "Vulkan frame resources require at least one frame");
    }

    void VulkanFrameResources::create()
    {
        AIKO_ASSERT(m_layout == VK_NULL_HANDLE, "Vulkan frame resources already created");

        const VkDescriptorSetLayoutBinding frameBinding =
        {
            .binding = abi::GraphicsFrameBinding,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr,
        };

        const VkDescriptorSetLayoutCreateInfo layoutInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = 1,
            .pBindings = &frameBinding,
        };

        const VkResult layoutResult = vkCreateDescriptorSetLayout(m_context.device(), &layoutInfo, nullptr, &m_layout);
        AIKO_ASSERT(layoutResult == VK_SUCCESS, "Failed to create frame descriptor set layout");

        const VkDescriptorPoolSize poolSize =
        {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = MaxBindingsPerFrame,
        };

        for (uint32_t frame = 0; frame < m_pools.size(); ++frame)
        {
            const VkDescriptorPoolCreateInfo poolInfo =
            {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                .flags = 0,
                .maxSets = MaxBindingsPerFrame,
                .poolSizeCount = 1,
                .pPoolSizes = &poolSize,
            };

            const VkResult result = vkCreateDescriptorPool(m_context.device(), &poolInfo, nullptr, &m_pools[frame]);
            AIKO_ASSERT(result == VK_SUCCESS, "Failed to create frame descriptor pool");

            m_bindings[frame].reserve(MaxBindingsPerFrame);
        }
    }

    const VulkanFrameBinding& VulkanFrameResources::allocate(uint32_t frameIndex, const VulkanFrameUbo& ubo)
    {
        AIKO_ASSERT(frameIndex < m_bindings.size(), "Invalid frame binding frame");
        AIKO_ASSERT(m_layout != VK_NULL_HANDLE,"Frame descriptor layout is invalid");
        AIKO_ASSERT(m_pools[frameIndex] != VK_NULL_HANDLE, "Frame descriptor pool is invalid");

        auto& bindings = m_bindings[frameIndex];
        AIKO_ASSERT(bindings.size() < MaxBindingsPerFrame, "Exceeded frame bindings per frame");

        VulkanFrameBinding binding{};

        const VkDeviceSize bufferSize = sizeof(VulkanFrameUbo);

        const std::array<uint32_t, 2> queueFamilies =
            {
                m_context.graphicsQueueFamily(),
                m_context.computeQueueFamily(),
            };

        if (
            m_context.hasDedicatedComputeQueue()
        )
        {
            m_context.createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, binding.uniformBuffer, binding.uniformMemory, VK_SHARING_MODE_CONCURRENT, queueFamilies.data(), static_cast<uint32_t>(queueFamilies.size()));
        }
        else
        {
            m_context.createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, binding.uniformBuffer, binding.uniformMemory);
        }

        const VkResult mapResult = vkMapMemory( m_context.device(), binding.uniformMemory, 0, bufferSize, 0, &binding.uniformMapped);
        AIKO_ASSERT(mapResult == VK_SUCCESS, "Failed to map frame uniform buffer");

        std::memcpy(binding.uniformMapped, &ubo, sizeof(ubo));

        const VkDescriptorSetAllocateInfo allocInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = m_pools[frameIndex],
            .descriptorSetCount = 1,
            .pSetLayouts = &m_layout,
        };

        const VkResult allocResult = vkAllocateDescriptorSets(m_context.device(), &allocInfo, &binding.descriptorSet);
        AIKO_ASSERT(allocResult == VK_SUCCESS, "Failed to allocate frame descriptor set");

        const VkDescriptorBufferInfo bufferInfo =
        {
            .buffer = binding.uniformBuffer,
            .offset = 0,
            .range = sizeof(VulkanFrameUbo),
        };

        const VkWriteDescriptorSet write =
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = binding.descriptorSet,
            .dstBinding = abi::GraphicsFrameBinding,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &bufferInfo,
        };

        vkUpdateDescriptorSets(m_context.device(),1,&write,0,nullptr);

        bindings.push_back(binding);

        return bindings.back();
    }

    const VulkanFrameBinding& VulkanFrameResources::latest(uint32_t frameIndex) const
    {
        AIKO_ASSERT(frameIndex < m_bindings.size(), "Invalid frame binding frame");
        const auto& bindings = m_bindings[frameIndex];
        AIKO_ASSERT(bindings.empty() == false, "Frame binding is not available");
        return bindings.back();
    }

    void VulkanFrameResources::destroyBindings(uint32_t frameIndex)
    {
        AIKO_ASSERT(frameIndex < m_bindings.size(), "Invalid frame binding frame");

        VkDevice device = m_context.device();

        for (VulkanFrameBinding& binding : m_bindings[frameIndex])
        {
            if (binding.uniformMapped != nullptr)
            {
                vkUnmapMemory(device, binding.uniformMemory);
                binding.uniformMapped = nullptr;
            }

            if (binding.uniformBuffer != VK_NULL_HANDLE)
            {
                vkDestroyBuffer(device, binding.uniformBuffer, nullptr);
                binding.uniformBuffer = VK_NULL_HANDLE;
            }

            if (binding.uniformMemory != VK_NULL_HANDLE)
            {
                vkFreeMemory(device, binding.uniformMemory, nullptr);
                binding.uniformMemory = VK_NULL_HANDLE;
            }
        }

        m_bindings[frameIndex].clear();
    }

    void VulkanFrameResources::resetFrame(uint32_t frameIndex)
    {
        AIKO_ASSERT(frameIndex < m_pools.size(), "Invalid frame binding frame");
        AIKO_ASSERT(m_pools[frameIndex] != VK_NULL_HANDLE, "Frame descriptor pool is invalid");
        destroyBindings(frameIndex);
        const VkResult result = vkResetDescriptorPool(m_context.device(), m_pools[frameIndex], 0);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to reset frame descriptor pool");
    }

    void VulkanFrameResources::destroy()
    {
        VkDevice device = m_context.device();

        for (uint32_t frame = 0; frame < m_pools.size(); ++frame)
        {
            destroyBindings(frame);
            if (m_pools[frame] != VK_NULL_HANDLE)
            {
                vkDestroyDescriptorPool(device, m_pools[frame], nullptr);
                m_pools[frame] = VK_NULL_HANDLE;
            }
        }

        if (m_layout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(device, m_layout, nullptr);
            m_layout = VK_NULL_HANDLE;
        }
    }
}
