#include "vulkan_shadow_resources.h"

#include "vulkan_context.h"
#include "vulkan_descriptor_abi.h"

namespace aiko::renderer::vulkan
{
    VulkanShadowResources::VulkanShadowResources(VulkanContext& context, uint32_t frameCount)
        : m_context(context)
        , m_frames(frameCount)
    {
        AIKO_ASSERT(frameCount > 0, "Shadow resources require at least one frame");
    }

    void VulkanShadowResources::create(VkDescriptorSetLayout layout)
    {
        AIKO_ASSERT(m_layout == VK_NULL_HANDLE, "Shadow resources already initialized");
        AIKO_ASSERT(layout != VK_NULL_HANDLE, "Shadow descriptor layout is invalid");

        m_layout = layout;

        const VkDevice device = m_context.device();

        const VkDescriptorPoolSize poolSizes[] =
        {
            {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
            },
            {
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
            },
        };

        for (FrameResources& frame : m_frames)
        {
            const VkDescriptorPoolCreateInfo poolInfo{
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                .maxSets = 1,
                .poolSizeCount = 2,
                .pPoolSizes = poolSizes,
            };

            AIKO_ASSERT(vkCreateDescriptorPool(device, &poolInfo, nullptr, &frame.pool) == VK_SUCCESS, "Failed to create shadow descriptor pool");

            const VkDescriptorSetAllocateInfo allocInfo{
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                .descriptorPool = frame.pool,
                .descriptorSetCount = 1,
                .pSetLayouts = &m_layout,
            };

            AIKO_ASSERT(vkAllocateDescriptorSets(device, &allocInfo, &frame.descriptorSet ) == VK_SUCCESS, "Failed to allocate shadow descriptor set");

            m_context.createBufferOrAbort(sizeof(VulkanShadowUbo), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, frame.uniformBuffer, frame.uniformMemory);

            AIKO_ASSERT(vkMapMemory( device, frame.uniformMemory, 0, sizeof(VulkanShadowUbo), 0, &frame.uniformMapped ) == VK_SUCCESS, "Failed to map shadow uniform buffer");

            const VkDescriptorBufferInfo bufferInfo{
                .buffer = frame.uniformBuffer,
                .offset = 0,
                .range = sizeof(VulkanShadowUbo),
            };

            const VkWriteDescriptorSet write{
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = frame.descriptorSet,
                .dstBinding = abi::ShadowUboBinding,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pBufferInfo = &bufferInfo,
            };

            vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
        }
    }

    void VulkanShadowResources::destroy()
    {
        const VkDevice device = m_context.device();

        for (FrameResources& frame : m_frames)
        {
            if (frame.uniformMapped != nullptr)
            {
                vkUnmapMemory(device, frame.uniformMemory);
                frame.uniformMapped = nullptr;
            }

            if (frame.uniformBuffer != VK_NULL_HANDLE)
            {
                vkDestroyBuffer(device, frame.uniformBuffer, nullptr);
                frame.uniformBuffer = VK_NULL_HANDLE;
            }

            if (frame.uniformMemory != VK_NULL_HANDLE)
            {
                vkFreeMemory(device, frame.uniformMemory, nullptr);
                frame.uniformMemory = VK_NULL_HANDLE;
            }

            if (frame.pool != VK_NULL_HANDLE)
            {
                vkDestroyDescriptorPool(device, frame.pool, nullptr);
                frame.pool = VK_NULL_HANDLE;
                frame.descriptorSet = VK_NULL_HANDLE;
            }
        }

        m_layout = VK_NULL_HANDLE;
    }

    VkDescriptorSet VulkanShadowResources::descriptorSet(uint32_t frameIndex) const
    {
        AIKO_ASSERT(frameIndex < m_frames.size(), "Invalid shadow frame index");
        return m_frames[frameIndex].descriptorSet;
    }
}
