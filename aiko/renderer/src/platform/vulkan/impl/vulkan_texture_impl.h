#pragma once

#include <aiko_types.h>

#include <types/texture_types.h>
#include "interfaces/itexture_impl.h"

namespace aiko::renderer::vulkan
{

    class VulkanTextureImpl : public interfaces::ITextureImpl
    {
    public:

        VulkanTextureImpl();
        virtual ~VulkanTextureImpl() override;

        virtual RenderResourceId id() const override;
        virtual void use() override;
        virtual void unuse() override;
        virtual bool isValid() const override;
        virtual TextureInfo getInfo() override;

        // load
        virtual void create(const TextureDesc&) override;
        virtual void unload() override;

        // Modify
        virtual void update(const TextureAsset& asset) override;
        virtual void setPixels(const vector<Color>& pixels) override;

        void setLayout(VkImageLayout layout) { m_layout = layout; }

        VkImage image() const { return m_image; }
        VkImageView imageView() const { return m_view; }
        VkFormat format() const { return m_vkFormat; }
        VkSampler sampler() const { return m_sampler; }
        VkImageLayout layout() const { return m_layout; }

    private:

        VkImage m_image = VK_NULL_HANDLE;
        VkDeviceMemory m_memory = VK_NULL_HANDLE;
        VkImageView m_view = VK_NULL_HANDLE;
        VkSampler m_sampler = VK_NULL_HANDLE;
        TextureInfo m_info = {};
        VkFormat m_vkFormat = VK_FORMAT_UNDEFINED;
        VkImageLayout m_layout = VK_IMAGE_LAYOUT_UNDEFINED;
        uint32_t m_mipLevels = 1;

        VkFormat convertToVkFormat(const TextureFormat format);

    };
}
