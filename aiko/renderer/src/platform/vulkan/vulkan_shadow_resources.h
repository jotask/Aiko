#pragma once

#include <cstdint>
#include <vector>
#include <array>

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

        void resetFrame(uint32_t frameIndex);
        void updateFallback(uint32_t frameIndex, VkImageView imageView, VkSampler sampler);

        void update(uint32_t frameIndex, const VulkanShadowUbo& uniforms, VkImageView depthView, VkSampler depthSampler);

        VkDescriptorSet descriptorSet(uint32_t frameIndex) const;

    private:
        struct Binding
        {
            VkDescriptorPool pool = VK_NULL_HANDLE;
            VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

            VkBuffer uniformBuffer = VK_NULL_HANDLE;
            VkDeviceMemory uniformMemory = VK_NULL_HANDLE;
            void* uniformMapped = nullptr;
        };

        struct FrameResources
        {
            Binding fallback;
            Binding active;

            VkDescriptorSet selectedSet = VK_NULL_HANDLE;
        };

        void createBinding(VkDescriptorSetLayout layout, Binding& binding);
        void destroyBinding(Binding& binding);

        VulkanContext& m_context;
        VkDescriptorSetLayout m_layout = VK_NULL_HANDLE;
        std::vector<FrameResources> m_frames;
    };
}
