#pragma once

#include <unordered_map>
#include <vector>

#include <volk.h>

#include "aiko_types.h"
#include "core/utils.h"
#include "models/material.h"

namespace aiko::renderer::vulkan
{
    struct CachedTextureBinding
    {
        uint32_t binding = 0;

        VkImageView imageView = VK_NULL_HANDLE;
        VkSampler sampler = VK_NULL_HANDLE;

        bool hasTexture = false;

        bool operator==(const CachedTextureBinding& other) const = default;
    };

    struct CachedMaterialBinding
    {
        VkBuffer uniformBuffer = VK_NULL_HANDLE;
        VkDeviceMemory uniformMemory = VK_NULL_HANDLE;

        void* uniformMapped = nullptr;
        VkDeviceSize uniformSize = 0;

        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

        std::vector<CachedTextureBinding> textures;
    };

    struct MaterialTextureBindingKey
    {
        uint32_t binding = 0;

        AssetId textureId = InvalidAssetId;
        const Texture* runtimeTexture = nullptr;

        SamplerState sampler{};

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

        bool operator==(const MaterialBindingKey& other) const
        {
            return shaderId == other.shaderId && textures == other.textures && uniformData == other.uniformData;
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
            }

            for (const uint8_t value : key.uniformData)
            {
                utils::hashCombine(std::hash<uint8_t>{}(value), seed);
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
}
