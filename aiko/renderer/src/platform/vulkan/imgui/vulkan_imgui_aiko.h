#pragma once

#include "imgui/aiko_imgui.h"
#include "platform/vulkan/vulkan_sampler_cache.h"
#include <core/utils.h>

#include <volk.h>

#include <unordered_map>
#include <memory>

namespace aiko::renderer::vulkan
{

    class VulkanImguiImpl : public AikoImguiImpl
    {
    public:
        virtual void init(const ViewId id, GLFWwindow*) override;
        virtual void beginFrame(const ViewId id, int width, int height) override;
        virtual void endFrame(const ViewId id, int width, int height) override;
        virtual void dispose() override;
        virtual ImguiTextureId textureId(const interfaces::ITextureImpl& texture, const SamplerState& sampler) override;

    private:

        struct TextureBinding
        {
            VkImageView imageView = VK_NULL_HANDLE;
            VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
        };

        struct TextureBindingKey
        {
            const interfaces::ITextureImpl* texture = nullptr;
            SamplerState sampler{};

            bool operator==(const TextureBindingKey& other) const = default;
        };

        struct TextureBindingKeyHash
        {
            size_t operator()(const TextureBindingKey& key) const
            {
                size_t seed = 0;
                utils::hashCombine(std::hash<const interfaces::ITextureImpl*>{}(key.texture), seed);
                utils::hashCombine(SamplerStateHash{}(key.sampler), seed);
                return seed;
            }
        };

        std::unique_ptr<VulkanSamplerCache> m_samplerCache;

        std::unordered_map<TextureBindingKey, TextureBinding, TextureBindingKeyHash> m_textureBindings;
    };

}

