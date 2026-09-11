#pragma once

#include "imgui/aiko_imgui.h"
#include <unordered_map>
#include <volk.h>

namespace aiko::renderer::vulkan
{

    class VulkanImguiImpl : public AikoImguiImpl
    {
    public:
        virtual void init(const ViewId id, GLFWwindow*) override;
        virtual void beginFrame(const ViewId id, int width, int height) override;
        virtual void endFrame(const ViewId id, int width, int height) override;
        virtual void dispose() override;
        virtual ImguiTextureId textureId(const interfaces::ITextureImpl& texture) override;

    private:

        struct TextureBinding
        {
            VkImageView imageView = VK_NULL_HANDLE;
            VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
        };

        VkSampler m_textureSampler = VK_NULL_HANDLE;

        std::unordered_map<const interfaces::ITextureImpl*, TextureBinding> m_textureBindings;
    };

}

