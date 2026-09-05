#pragma once

#include "types/render_types.h"


#include <vector>
#include <optional>
#include <limits>
#include <array>
#include <cstddef>

#include <volk.h>

#include <math/math.h>
#include <types/color.h>

namespace aiko
{

    struct VulkanVertex
    {
        vec3 position;
        vec3 normal;
        vec2 uv;
        Color color;

        static VkVertexInputBindingDescription bindingDescription()
        {
            return {
                .binding = 0,
                .stride = sizeof(VulkanVertex),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
            };
        }

        static std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions()
        {
            std::array<VkVertexInputAttributeDescription, 4> attributes{};

            attributes[0] = { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VulkanVertex, position) };
            attributes[1] = { 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VulkanVertex, normal) };
            attributes[2] = { 2, 0, VK_FORMAT_R32G32_SFLOAT,    offsetof(VulkanVertex, uv) };
            attributes[3] = { 3, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(VulkanVertex, color) };

            return attributes;
        }

    };

    struct VulkanInstanceData
    {
        vec4 position;
        vec4 rotation;
        vec4 scale;
        vec4 color;
    };

    static_assert(sizeof(VulkanInstanceData) == sizeof(vec4) * 4);
    static_assert(offsetof(VulkanInstanceData, position) == 0);
    static_assert(offsetof(VulkanInstanceData, rotation) == 16);
    static_assert(offsetof(VulkanInstanceData, scale) == 32);
    static_assert(offsetof(VulkanInstanceData, color) == 48);

    struct VulkanFrameUbo
    {
        alignas(16) mat4 u_view;
        alignas(16) mat4 u_projection;
        alignas(16) mat4 u_viewProj;
        alignas(16) vec4 u_cameraPos;
        alignas(16) vec4 u_time;
        alignas(16) vec4 u_ambientColor;
        alignas(16) vec4 u_ambientIntensity;
        alignas(16) vec4 u_lightCount;
        alignas(16) std::array<vec4, MaxFrameLights> u_lightType;
        alignas(16) std::array<vec4, MaxFrameLights> u_lightPosRange;
        alignas(16) std::array<vec4, MaxFrameLights> u_lightDir;
        alignas(16) std::array<vec4, MaxFrameLights> u_lightColorInt;
        alignas(16) std::array<vec4, MaxFrameLights> u_lightSpotCos;
    };

    struct VulkanImageState
    {
        VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkPipelineStageFlags stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        VkAccessFlags access = 0;
        uint32_t queueFamily = VK_QUEUE_FAMILY_IGNORED;
    };

    struct VulkanBufferState
    {
        VkPipelineStageFlags stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        VkAccessFlags access = 0;
        uint32_t queueFamily = VK_QUEUE_FAMILY_IGNORED;
    };

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        std::optional<uint32_t> computeFamily;
        [[nodiscard]]
        bool isComplete() const
        {
            return graphicsFamily.has_value() && presentFamily.has_value() && computeFamily.has_value();
        }
    };

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilties;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct UniformBufferObject
    {
        mat4 view;
        mat4 proj;
    };

    struct PushConstantData
    {
        mat4 model;
    };

    const std::vector<const char*> validationLayers =
    {
        "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char*> deviceExtensions =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    #ifdef AIKO_DEBUG
    constexpr bool enableValidationLayers = true;
    #else
    constexpr bool enableValidationLayers = false;
    #endif

    constexpr uint32_t InvalidMemoryType = std::numeric_limits<uint32_t>::max();

}