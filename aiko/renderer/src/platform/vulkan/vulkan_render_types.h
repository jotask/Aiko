#pragma once

#include <unordered_map>
#include <vector>

#include <volk.h>

#include "aiko_types.h"
#include "core/utils.h"
#include "models/material.h"

namespace aiko::renderer::vulkan
{
    struct VulkanMaterialTextureState
    {
        uint32_t binding = 0;

        VkImageView imageView = VK_NULL_HANDLE;
        VkSampler sampler = VK_NULL_HANDLE;
        VkImageLayout imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        bool hasTexture = false;

        bool operator==(const VulkanMaterialTextureState& other) const = default;
    };

    struct VulkanMaterialBinding
    {
        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
        uint32_t dynamicOffset = 0;
    };

    struct VulkanMaterialUniformChunk
    {
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        void* mapped = nullptr;
        VkDeviceSize capacity = 0;
        VkDeviceSize used = 0;
    };

    struct MaterialDescriptorKey
    {
        VkBuffer uniformBuffer = VK_NULL_HANDLE;
        VkDeviceSize uniformRange = 0;
        std::vector<VulkanMaterialTextureState> textures;

        bool operator==(const MaterialDescriptorKey& other) const = default;
    };

    struct MaterialDescriptorKeyHash
    {
        size_t operator()(const MaterialDescriptorKey& key) const
        {
            size_t seed = 0;
            utils::hashCombine(std::hash<VkBuffer>{}(key.uniformBuffer), seed);
            utils::hashCombine(std::hash<VkDeviceSize>{}(key.uniformRange), seed);
            for (const auto& texture : key.textures)
            {
                utils::hashCombine(std::hash<uint32_t>{}(texture.binding), seed);
                utils::hashCombine(std::hash<VkImageView>{}(texture.imageView), seed);
                utils::hashCombine(std::hash<VkSampler>{}(texture.sampler), seed);
                utils::hashCombine(std::hash<uint32_t>{}(static_cast<uint32_t>(texture.imageLayout)), seed);
                utils::hashCombine(std::hash<bool>{}(texture.hasTexture), seed);
            }
            return seed;
        }
    };

    struct MaterialTextureBindingKey
    {
        uint32_t binding = 0;

        AssetId textureId = InvalidAssetId;
        const Texture* runtimeTexture = nullptr;

        SamplerState sampler{};
        TextureColorSpace colorSpace = TextureColorSpace::Linear;

        bool operator==(const MaterialTextureBindingKey& other) const = default;
    };

    struct SamplerStateHash
    {
        size_t operator()(const SamplerState& state) const
        {
            size_t seed = 0;
            utils::hashCombine(std::hash<uint32_t>{}(static_cast<uint32_t>(state.minFilter)), seed);
            utils::hashCombine(std::hash<uint32_t>{}(static_cast<uint32_t>(state.magFilter)), seed);
            utils::hashCombine(std::hash<uint32_t>{}(static_cast<uint32_t>(state.mipFilter)), seed);
            utils::hashCombine(std::hash<uint32_t>{}(static_cast<uint32_t>(state.wrapU)), seed);
            utils::hashCombine(std::hash<uint32_t>{}(static_cast<uint32_t>(state.wrapV)), seed);
            return seed;
        }
    };

    struct MaterialBindingKey
    {
        AssetId shaderId = InvalidAssetId;

        std::vector<MaterialTextureBindingKey> textures;
        std::vector<uint8_t> uniformData;
        std::vector<VulkanMaterialTextureState> resolvedTextures;

        bool operator==(const MaterialBindingKey& other) const
        {
            return shaderId == other.shaderId && textures == other.textures && uniformData == other.uniformData && resolvedTextures == other.resolvedTextures;
        }
    };

    struct MaterialBindingKeyHash
    {
        size_t operator()(const MaterialBindingKey& key) const
        {
            size_t seed = 0;

            utils::hashCombine(std::hash<AssetId>{}(key.shaderId), seed);

            for (const MaterialTextureBindingKey& texture : key.textures)
            {
                utils::hashCombine(std::hash<uint32_t>{}(texture.binding), seed);
                utils::hashCombine(std::hash<AssetId>{}(texture.textureId), seed);
                utils::hashCombine(std::hash<const Texture*>{}(texture.runtimeTexture), seed);
                utils::hashCombine(SamplerStateHash{}(texture.sampler), seed);
                utils::hashCombine(std::hash<uint32_t>{}(static_cast<uint32_t>(texture.colorSpace)), seed);
            }

            for (const uint8_t value : key.uniformData)
            {
                utils::hashCombine(std::hash<uint8_t>{}(value), seed);
            }
            for (const auto& texture : key.resolvedTextures)
            {
                utils::hashCombine(std::hash<uint32_t>{}(texture.binding), seed);
                utils::hashCombine(std::hash<VkImageView>{}(texture.imageView), seed);
                utils::hashCombine(std::hash<VkSampler>{}(texture.sampler), seed);
                utils::hashCombine(std::hash<uint32_t>{}(static_cast<uint32_t>(texture.imageLayout)), seed);
                utils::hashCombine(std::hash<bool>{}(texture.hasTexture), seed);
            }

            return seed;
        }
    };

    struct VulkanFrameBinding
    {
        VkBuffer uniformBuffer = VK_NULL_HANDLE;
        VkDeviceMemory uniformMemory = VK_NULL_HANDLE;
        void* uniformMapped = nullptr;
        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
    };

    struct VulkanMaterialFrameResources
    {
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        std::unordered_map<MaterialDescriptorKey, VkDescriptorSet, MaterialDescriptorKeyHash> descriptors;
        std::vector<VulkanMaterialUniformChunk> uniformChunks;

        std::unordered_map<MaterialBindingKey, VulkanMaterialBinding, MaterialBindingKeyHash> bindings;

        std::vector<VkDescriptorSet> allocatedDescriptorSets;
        uint32_t nextDescriptorSet = 0;
    };
}
