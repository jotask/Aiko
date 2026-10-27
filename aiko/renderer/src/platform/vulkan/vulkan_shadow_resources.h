#pragma once

#include <cstdint>
#include <vector>

#include <volk.h>
#include <math/math.h>

namespace aiko::renderer::vulkan
{
    class VulkanContext;

    // Matches the ShadowUbo declaration in aiko_shadow.glsl.
    struct VulkanShadowUbo
    {
        mat4 lightViewProjection{1.0f};
        vec4 shadowParams{0.0f};
    };

    class VulkanShadowResources final
    {
    public:
        VulkanShadowResources(VulkanContext& context, uint32_t frameCount);

        VulkanShadowResources(const VulkanShadowResources&) = delete;
        VulkanShadowResources& operator=(const VulkanShadowResources&) = delete;

        void create(VkDescriptorSetLayout layout);
        void destroy();

        void update(uint32_t frameIndex, const VulkanShadowUbo& uniforms, VkImageView depthView, VkSampler depthSampler);

        VkDescriptorSet descriptorSet(uint32_t frameIndex) const;

    private:
        struct FrameResources
        {
            VkDescriptorPool pool = VK_NULL_HANDLE;
            VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

            VkBuffer uniformBuffer = VK_NULL_HANDLE;
            VkDeviceMemory uniformMemory = VK_NULL_HANDLE;
            void* uniformMapped = nullptr;
        };

        VulkanContext& m_context;
        VkDescriptorSetLayout m_layout = VK_NULL_HANDLE;
        std::vector<FrameResources> m_frames;
    };
}
