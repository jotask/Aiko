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

        const std::array<VkDescriptorPoolSize, 2> poolSizes =
        {
            VkDescriptorPoolSize
            {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
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

        auto& bindings = resources.bindings;

        if (const auto it = bindings.find(key); it != bindings.end())
        {
            return it->second;
        }

        const VkDeviceSize requiredUniformSize = static_cast<VkDeviceSize>(key.uniformData.size());

        AIKO_ASSERT(requiredUniformSize > 0, "Material uniform buffer has zero size");

        VulkanMaterialBinding binding{};

        auto reusable = std::find_if(
            resources.recycledBindings.begin(),
            resources.recycledBindings.end(),
            [requiredUniformSize](const VulkanMaterialBinding& candidate)
            {
                return candidate.uniformSize >= requiredUniformSize;
            }
        );

        if (reusable != resources.recycledBindings.end())
        {
            binding = std::move(*reusable);
            if (reusable != resources.recycledBindings.end() - 1)
            {
                *reusable = std::move(resources.recycledBindings.back());
            }
            resources.recycledBindings.pop_back();
        }
        else
        {
            binding.uniformSize = requiredUniformSize;

            m_context.createBuffer(binding.uniformSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, binding.uniformBuffer, binding.uniformMemory);

            const VkResult mapResult = vkMapMemory(m_context.device(), binding.uniformMemory, 0, binding.uniformSize, 0, &binding.uniformMapped);

            AIKO_ASSERT(mapResult == VK_SUCCESS, "Failed to map material uniform buffer");
        }

        std::memcpy(binding.uniformMapped, key.uniformData.data(), key.uniformData.size());

        const VkDescriptorSetAllocateInfo allocInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = resources.descriptorPool,
            .descriptorSetCount = 1,
            .pSetLayouts = &m_materialLayout,
        };

        const VkResult allocResult = vkAllocateDescriptorSets(m_context.device(), &allocInfo, &binding.descriptorSet);
        AIKO_ASSERT(allocResult == VK_SUCCESS, "Failed to allocate material descriptor set");

        const VkDescriptorBufferInfo bufferInfo =
        {
            .buffer = binding.uniformBuffer,
            .offset = 0,
            .range = requiredUniformSize,
        };

        const VkWriteDescriptorSet bufferWrite =
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = binding.descriptorSet,
            .dstBinding = abi::MaterialUboBinding,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &bufferInfo,
        };

        vkUpdateDescriptorSets(m_context.device(), 1, &bufferWrite, 0, nullptr);

        auto [it, inserted] = bindings.emplace(key, std::move(binding));

        AIKO_ASSERT(inserted, "Failed to cache Vulkan material binding");

        return it->second;
    }

    void VulkanMaterialResources::resetFrame(uint32_t frame)
    {
        AIKO_FUNCTION_PROFILE

        AIKO_ASSERT(frame < m_frames.size(), "Invalid material frame index");

        VulkanMaterialFrameResources& resources = m_frames[frame];

        recycleBindings(frame);

        AIKO_ASSERT(resources.descriptorPool != VK_NULL_HANDLE, "Material descriptor pool is not initialized");

        const VkResult result = vkResetDescriptorPool(m_context.device(), resources.descriptorPool, 0);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to reset material descriptor pool");
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

    void VulkanMaterialResources::recycleBindings(uint32_t frame)
    {
        AIKO_FUNCTION_PROFILE

        AIKO_ASSERT(frame < m_frames.size(), "Invalid material frame index");

        VulkanMaterialFrameResources& resources = m_frames[frame];

        for (auto& [key, binding] : resources.bindings)
        {
            AIKO_UNUSED(key);

            // Descriptor sets become invalid after vkResetDescriptorPool().
            binding.descriptorSet = VK_NULL_HANDLE;

            // The next fresh descriptor set must receive texture writes even if
            // image/sampler state happens to match the previous frame.
            binding.textures.clear();

            resources.recycledBindings.push_back(std::move(binding));
        }

        resources.bindings.clear();
    }

    void VulkanMaterialResources::destroyBindings(uint32_t frame)
    {
        AIKO_FUNCTION_PROFILE

        AIKO_ASSERT(frame < m_frames.size(), "Invalid material frame index");

        VkDevice device = m_context.device();

        VulkanMaterialFrameResources& resources = m_frames[frame];

        const auto destroyBinding = [device](VulkanMaterialBinding& binding)
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
            };

        for (auto& [key, binding] : resources.bindings)
        {
            AIKO_UNUSED(key);
            destroyBinding(binding);
        }

        resources.bindings.clear();

        for (VulkanMaterialBinding& binding : resources.recycledBindings)
        {
            destroyBinding(binding);
        }

        resources.recycledBindings.clear();
    }
}
