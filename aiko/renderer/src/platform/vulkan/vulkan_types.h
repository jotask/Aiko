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

    struct VulkanFrameUbo
    {
        alignas(16) mat4 u_view;
        alignas(16) mat4 u_projection;
        alignas(16) mat4 u_viewProj;
        alignas(16) vec4 u_cameraPos;
        alignas(16) vec4 u_ambientColor;
        alignas(16) vec4 u_ambientIntensity;
        alignas(16) vec4 u_lightCount;
        alignas(16) std::array<vec4, MAX_LIGHTS> u_lightType;
        alignas(16) std::array<vec4, MAX_LIGHTS> u_lightPosRange;
        alignas(16) std::array<vec4, MAX_LIGHTS> u_lightDir;
        alignas(16) std::array<vec4, MAX_LIGHTS> u_lightColorInt;
        alignas(16) std::array<vec4, MAX_LIGHTS> u_lightSpotCos;
    };

    struct VulkanMaterialUbo
    {
        alignas(16) vec4 u_baseColor;
        alignas(16) vec4 u_flags;
        alignas(16) vec4 u_particleSizeLife;
        alignas(16) vec4 u_particleStartColor;
        alignas(16) vec4 u_particleEndColor;
        alignas(16) vec4 u_billboardParams;
        alignas(16) vec4 u_nbodyRender;
    };

    struct VulkanImageState
    {
        VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkPipelineStageFlags stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        VkAccessFlags access = 0;
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

    struct ComputeUniformBufferObject
    {
        float deltaTime;
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