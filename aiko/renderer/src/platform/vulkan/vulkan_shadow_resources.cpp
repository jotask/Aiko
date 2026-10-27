#include "vulkan_shadow_resources.h"

#include "vulkan_context.h"
#include "vulkan_descriptor_abi.h"

#include <cstring>

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
        AIKO_ASSERT(layout != VK_NULL_HANDLE, "Invalid shadow descriptor layout");

        m_layout = layout;

        for (FrameResources& frame : m_frames)
        {
            createBinding(layout, frame.fallback);
            createBinding(layout, frame.active);
            frame.selectedSet = frame.fallback.descriptorSet;
        }
    }

    void VulkanShadowResources::destroy()
    {
        for (FrameResources& frame : m_frames)
        {
            destroyBinding(frame.fallback);
            destroyBinding(frame.active);
            frame.selectedSet = VK_NULL_HANDLE;
        }

        m_layout = VK_NULL_HANDLE;
    }

    void VulkanShadowResources::resetFrame(uint32_t frameIndex)
    {
        AIKO_ASSERT(frameIndex < m_frames.size(), "Invalid shadow frame index");
        FrameResources& frame = m_frames[frameIndex];
        frame.selectedSet = frame.fallback.descriptorSet;
    }

    void VulkanShadowResources::updateFallback(uint32_t frameIndex, VkImageView imageView, VkSampler sampler)
    {
        AIKO_ASSERT(frameIndex < m_frames.size(), "Invalid shadow frame index");
        AIKO_ASSERT(imageView != VK_NULL_HANDLE, "Invalid fallback image view");
        AIKO_ASSERT(sampler != VK_NULL_HANDLE, "Invalid fallback sampler");

        Binding& binding = m_frames[frameIndex].fallback;

        // Disable shadow comparison in PBR.
        const VulkanShadowUbo uniforms{};

        std::memcpy(binding.uniformMapped, &uniforms, sizeof(uniforms));

        ShadowImages images{};

        for (VkDescriptorImageInfo& image : images)
        {
            image = {
                .sampler = sampler,
                .imageView = imageView,
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            };
        }

        const VkWriteDescriptorSet write{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = binding.descriptorSet,
            .dstBinding = abi::ShadowTextureBinding,
            .descriptorCount = MaxFrameLights,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .pImageInfo = images.data(),
        };

        vkUpdateDescriptorSets(m_context.device(), 1, &write, 0, nullptr);
    }

    void VulkanShadowResources::update(uint32_t frameIndex, const VulkanShadowUbo& uniforms, const ShadowImages& images)
    {
        AIKO_ASSERT(frameIndex < m_frames.size(), "Invalid shadow frame index");

        FrameResources& frame = m_frames[frameIndex];
        Binding& binding = frame.active;

        AIKO_ASSERT(binding.uniformMapped != nullptr, "Shadow uniform buffer is not mapped");
        AIKO_ASSERT(binding.descriptorSet != VK_NULL_HANDLE, "Shadow descriptor set is invalid");

        std::memcpy(binding.uniformMapped, &uniforms, sizeof(VulkanShadowUbo));

        for (const VkDescriptorImageInfo& image : images)
        {
            AIKO_ASSERT(image.imageView != VK_NULL_HANDLE && image.sampler != VK_NULL_HANDLE, "Invalid shadow image descriptor");
        }

        const VkWriteDescriptorSet write{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = binding.descriptorSet,
            .dstBinding = abi::ShadowTextureBinding,
            .descriptorCount = MaxFrameLights,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .pImageInfo = images.data(),
        };

        vkUpdateDescriptorSets(m_context.device(), 1, &write, 0, nullptr);

        frame.selectedSet = binding.descriptorSet;
    }

    VkDescriptorSet VulkanShadowResources::descriptorSet(uint32_t frameIndex) const
    {
        AIKO_ASSERT(frameIndex < m_frames.size(), "Invalid shadow frame index");
        const VkDescriptorSet selectedSet = m_frames[frameIndex].selectedSet;
        AIKO_ASSERT(selectedSet != VK_NULL_HANDLE, "Shadow descriptor set is not initialized");
        return selectedSet;
    }

    void VulkanShadowResources::createBinding(VkDescriptorSetLayout layout, Binding& binding)
    {
        const VkDevice device = m_context.device();

        AIKO_ASSERT(binding.pool == VK_NULL_HANDLE, "Shadow binding already initialized");

        const VkDescriptorPoolSize poolSizes[] =
        {
            {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
            },
            {
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = MaxFrameLights,
            },
        };

        const VkDescriptorPoolCreateInfo poolInfo{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = 1,
            .poolSizeCount = 2,
            .pPoolSizes = poolSizes,
        };

        AIKO_ASSERT(vkCreateDescriptorPool(device, &poolInfo, nullptr, &binding.pool) == VK_SUCCESS, "Failed to create shadow descriptor pool");

        const VkDescriptorSetAllocateInfo allocInfo{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = binding.pool,
            .descriptorSetCount = 1,
            .pSetLayouts = &layout,
        };

        AIKO_ASSERT(vkAllocateDescriptorSets(device, &allocInfo, &binding.descriptorSet) == VK_SUCCESS, "Failed to allocate shadow descriptor set");

        m_context.createBufferOrAbort(sizeof(VulkanShadowUbo), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, binding.uniformBuffer, binding.uniformMemory);

        AIKO_ASSERT(vkMapMemory( device, binding.uniformMemory, 0, sizeof(VulkanShadowUbo), 0, &binding.uniformMapped ) == VK_SUCCESS, "Failed to map shadow uniform buffer");

        const VkDescriptorBufferInfo bufferInfo{
            .buffer = binding.uniformBuffer,
            .offset = 0,
            .range = sizeof(VulkanShadowUbo),
        };

        const VkWriteDescriptorSet write{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = binding.descriptorSet,
            .dstBinding = abi::ShadowUboBinding,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &bufferInfo,
        };

        vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
    }

    void VulkanShadowResources::destroyBinding(Binding& binding)
    {
        const VkDevice device = m_context.device();

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

        if (binding.pool != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool(device, binding.pool, nullptr);
            binding.pool = VK_NULL_HANDLE;
        }

        binding.descriptorSet = VK_NULL_HANDLE;
    }
}
