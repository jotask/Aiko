#pragma once

#include <memory>
#include <array>

#include <aiko_types.h>

#include <types/texture_types.h>
#include "interfaces/itexture_impl.h"

#include "platform/vulkan/vulkan_types.h"

namespace aiko::renderer::vulkan
{

    class VulkanTextureImpl : public interfaces::ITextureImpl
    {
    public:

        VulkanTextureImpl();
        virtual ~VulkanTextureImpl() override;

        virtual RenderResourceId id() const override;
        virtual bool isValid() const override;
        virtual TextureInfo getInfo() override;

        // load
        virtual void create(const TextureDesc&) override;
        virtual void unload() override;

        // Modify
        virtual void update(const TextureAsset& asset) override;
        virtual void setPixels(const vector<Color>& pixels) override;

        VkImage image() const { return m_image; }
        VkImageView imageView() const { return m_view; }
        VkFormat format() const { return m_vkFormat; }

        const VulkanImageState& state() const { return m_state; }
        void setState(const VulkanImageState& state) { m_state = state; }
        VkImageLayout layout() const { return m_state.layout; }

        const VulkanImageState& faceState(uint32_t face) const
        {
            AIKO_ASSERT(m_info.dimension == TextureDimension::Cube, "Face state requires a cubemap");
            AIKO_ASSERT(face < 6, "Invalid cubemap face");
            return m_faceStates[face];
        }

        void setFaceState(uint32_t face, const VulkanImageState& state)
        {
            AIKO_ASSERT(m_info.dimension == TextureDimension::Cube, "Face state requires a cubemap");
            AIKO_ASSERT(face < 6, "Invalid cubemap face");
            m_faceStates[face] = state;
        }

        std::weak_ptr<const u8> lifetime() const { return m_lifetime; }

    private:

        VkImage m_image = VK_NULL_HANDLE;
        VkDeviceMemory m_memory = VK_NULL_HANDLE;
        VkImageView m_view = VK_NULL_HANDLE;
        TextureInfo m_info = {};
        VkFormat m_vkFormat = VK_FORMAT_UNDEFINED;
        uint32_t m_mipLevels = 1;

        VulkanImageState m_state{};
        std::array<VulkanImageState, 6> m_faceStates{};

        std::shared_ptr<const u8> m_lifetime;

        VkFormat convertToVkFormat(const TextureFormat format);

    };
}
