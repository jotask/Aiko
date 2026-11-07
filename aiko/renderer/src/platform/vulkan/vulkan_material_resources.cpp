#include "vulkan_material_resources.h"

#include <array>
#include <cstring>
#include <utility>
#include <algorithm>

#include "vulkan_context.h"
#include "vulkan_descriptor_abi.h"

#include <intrumentor/profiler.h>

namespace aiko::renderer::vulkan
{
    VulkanMaterialResources::VulkanMaterialResources(VulkanContext& context, uint32_t frameCount)
        : m_context(context)
        , m_frames(frameCount)
    {
        AIKO_ASSERT(frameCount > 0, "Vulkan material resources require at least one frame");
    }

    void VulkanMaterialResources::create(VkDescriptorSetLayout materialLayout)
    {
        AIKO_ASSERT(m_materialLayout == VK_NULL_HANDLE, "Vulkan material resources already created");
        AIKO_ASSERT(materialLayout != VK_NULL_HANDLE, "Material descriptor layout is invalid");

        m_materialLayout = materialLayout;
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(m_context.physicalDevice(), &properties);
        m_uniformAlignment = std::max<VkDeviceSize>(1, properties.limits.minUniformBufferOffsetAlignment);
        m_maxUniformRange = properties.limits.maxUniformBufferRange;

        const std::array<VkDescriptorPoolSize, 2> poolSizes =
        {
            VkDescriptorPoolSize
            {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
                .descriptorCount = MaxBindings,
            },
            VkDescriptorPoolSize
            {
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = MaxBindings * abi::MaxMaterialTextureBindings,
            },
        };

        const VkDescriptorPoolCreateInfo poolInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = MaxBindings,
            .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
            .pPoolSizes = poolSizes.data(),
        };

        for (VulkanMaterialFrameResources& frame : m_frames)
        {
            AIKO_ASSERT(frame.descriptorPool == VK_NULL_HANDLE, "Material descriptor pool already created");
            const VkResult result = vkCreateDescriptorPool(m_context.device(), &poolInfo, nullptr, &frame.descriptorPool);
            AIKO_ASSERT(result == VK_SUCCESS, "Failed to create material descriptor pool");
        }
    }

        VulkanMaterialBinding& VulkanMaterialResources::getOrCreate(uint32_t frame, const MaterialBindingKey& key)
    {
        AIKO_ASSERT(frame < m_frames.size(), "Invalid material binding frame");
        AIKO_ASSERT(m_materialLayout != VK_NULL_HANDLE, "Material descriptor layout is invalid");

        VulkanMaterialFrameResources& resources = m_frames[frame];
        if (const auto it = resources.bindings.find(key); it != resources.bindings.end())
        {
            return it->second;
        }

        const VkDeviceSize uniformSize = static_cast<VkDeviceSize>(key.uniformData.size());
        AIKO_ASSERT(uniformSize > 0, "Material uniform buffer has zero size");

        const VkDeviceSize alignment = m_uniformAlignment;
        AIKO_ASSERT((alignment & (alignment - 1)) == 0, "Material uniform alignment must be a power of two");
        AIKO_ASSERT(uniformSize <= m_maxUniformRange, "Material UBO exceeds Vulkan maxUniformBufferRange");

        auto alignUp = [alignment](VkDeviceSize value)
        {
            return (value + alignment - 1) & ~(alignment - 1);
        };

        VulkanMaterialUniformChunk* chunk = nullptr;
        VkDeviceSize offset = 0;

        for (VulkanMaterialUniformChunk& candidate : resources.uniformChunks)
        {
            const VkDeviceSize aligned = alignUp(candidate.used);
            if (aligned <= candidate.capacity && uniformSize <= candidate.capacity - aligned &&
                aligned <= UINT32_MAX && uniformSize <= static_cast<VkDeviceSize>(UINT32_MAX) - aligned)
            {
                chunk = &candidate;
                offset = aligned;
                break;
            }
        }

        if (chunk == nullptr)
        {
            static constexpr VkDeviceSize DefaultMaterialChunkSize = 4ull * 1024ull * 1024ull;
            const VkDeviceSize capacity = std::max(DefaultMaterialChunkSize, alignUp(uniformSize));
            AIKO_ASSERT(capacity <= UINT32_MAX, "Material uniform chunk exceeds dynamic offset limit");

            VulkanMaterialUniformChunk created{};
            created.capacity = capacity;
            m_context.createBufferOrAbort(
                capacity,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                created.buffer,
                created.memory
            );
            const VkResult mapResult = vkMapMemory(m_context.device(), created.memory, 0, capacity, 0, &created.mapped);
            AIKO_ASSERT(mapResult == VK_SUCCESS, "Failed to map shared material uniform buffer");
            resources.uniformChunks.push_back(created);
            chunk = &resources.uniformChunks.back();
            offset = 0;
        }

        std::memcpy(static_cast<uint8_t*>(chunk->mapped) + offset, key.uniformData.data(), key.uniformData.size());
        chunk->used = offset + uniformSize;

        MaterialDescriptorKey descriptorKey{};
        descriptorKey.uniformBuffer = chunk->buffer;
        descriptorKey.uniformRange = uniformSize;
        descriptorKey.textures = key.resolvedTextures;

        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
        if (const auto it = resources.descriptors.find(descriptorKey); it != resources.descriptors.end())
        {
            descriptorSet = it->second;
        }
        else
        {
            // Keep the already committed 64-set descriptor batching allocator.
            if (resources.nextDescriptorSet == resources.allocatedDescriptorSets.size())
            {
                static constexpr uint32_t DescriptorBatchSize = 64;
                const uint32_t allocatedCount = static_cast<uint32_t>(resources.allocatedDescriptorSets.size());
                AIKO_ASSERT(allocatedCount < MaxBindings, "Material descriptor pool exhausted");
                const uint32_t count = std::min(DescriptorBatchSize, MaxBindings - allocatedCount);

                std::array<VkDescriptorSetLayout, DescriptorBatchSize> layouts{};
                layouts.fill(m_materialLayout);
                std::array<VkDescriptorSet, DescriptorBatchSize> sets{};
                const VkDescriptorSetAllocateInfo allocInfo =
                {
                    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                    .descriptorPool = resources.descriptorPool,
                    .descriptorSetCount = count,
                    .pSetLayouts = layouts.data(),
                };
                const VkResult result = vkAllocateDescriptorSets(m_context.device(), &allocInfo, sets.data());
                AIKO_ASSERT(result == VK_SUCCESS, "Failed to allocate material descriptor batch");
                resources.allocatedDescriptorSets.insert(
                    resources.allocatedDescriptorSets.end(), sets.begin(), sets.begin() + count
                );
            }
            descriptorSet = resources.allocatedDescriptorSets[resources.nextDescriptorSet++];

            const VkDescriptorBufferInfo bufferInfo =
            {
                .buffer = chunk->buffer,
                .offset = 0,
                .range = uniformSize,
            };
            std::vector<VkDescriptorImageInfo> imageInfos;
            imageInfos.reserve(key.resolvedTextures.size());
            std::vector<VkWriteDescriptorSet> writes;
            writes.reserve(1 + key.resolvedTextures.size());
            writes.push_back({
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = descriptorSet,
                .dstBinding = abi::MaterialUboBinding,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
                .pBufferInfo = &bufferInfo,
            });

            for (const VulkanMaterialTextureState& texture : key.resolvedTextures)
            {
                imageInfos.push_back({
                    .sampler = texture.sampler,
                    .imageView = texture.imageView,
                    .imageLayout = texture.imageLayout,
                });
                writes.push_back({
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = descriptorSet,
                    .dstBinding = texture.binding,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .pImageInfo = &imageInfos.back(),
                });
            }
            vkUpdateDescriptorSets(m_context.device(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
            resources.descriptors.emplace(std::move(descriptorKey), descriptorSet);
        }

        VulkanMaterialBinding binding{};
        binding.descriptorSet = descriptorSet;
        binding.dynamicOffset = static_cast<uint32_t>(offset);
        const auto [it, inserted] = resources.bindings.emplace(key, binding);
        AIKO_ASSERT(inserted, "Failed to cache Vulkan material binding");
        return it->second;
    }

        void VulkanMaterialResources::resetFrame(uint32_t frame)
    {
        AIKO_FUNCTION_PROFILE

        AIKO_ASSERT(frame < m_frames.size(), "Invalid material frame index");
        VulkanMaterialFrameResources& resources = m_frames[frame];
        AIKO_ASSERT(resources.descriptorPool != VK_NULL_HANDLE, "Material descriptor pool is not initialized");

        // The frame slot has completed on the GPU before this reset.
        resources.bindings.clear();
        resources.descriptors.clear();
        for (VulkanMaterialUniformChunk& chunk : resources.uniformChunks)
        {
            chunk.used = 0;
        }

        const VkResult result = vkResetDescriptorPool(m_context.device(), resources.descriptorPool, 0);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to reset material descriptor pool");
        resources.allocatedDescriptorSets.clear();
        resources.nextDescriptorSet = 0;
    }

    void VulkanMaterialResources::destroy()
    {
        VkDevice device = m_context.device();

        for (uint32_t frame = 0; frame < m_frames.size(); ++frame)
        {
            VulkanMaterialFrameResources& resources = m_frames[frame];
            destroyBindings(frame);
            if (resources.descriptorPool != VK_NULL_HANDLE)
            {
                vkDestroyDescriptorPool(device, resources.descriptorPool, nullptr);
                resources.descriptorPool = VK_NULL_HANDLE;
            }
        }
        m_materialLayout = VK_NULL_HANDLE;
    }

    

        void VulkanMaterialResources::destroyBindings(uint32_t frame)
    {
        AIKO_FUNCTION_PROFILE

        AIKO_ASSERT(frame < m_frames.size(), "Invalid material frame index");
        VulkanMaterialFrameResources& resources = m_frames[frame];
        resources.bindings.clear();
        resources.descriptors.clear();
        resources.allocatedDescriptorSets.clear();
        resources.nextDescriptorSet = 0;

        const VkDevice device = m_context.device();
        for (VulkanMaterialUniformChunk& chunk : resources.uniformChunks)
        {
            if (chunk.mapped != nullptr)
            {
                vkUnmapMemory(device, chunk.memory);
            }
            if (chunk.buffer != VK_NULL_HANDLE)
            {
                vkDestroyBuffer(device, chunk.buffer, nullptr);
            }
            if (chunk.memory != VK_NULL_HANDLE)
            {
                vkFreeMemory(device, chunk.memory, nullptr);
            }
        }
        resources.uniformChunks.clear();
    }
}
