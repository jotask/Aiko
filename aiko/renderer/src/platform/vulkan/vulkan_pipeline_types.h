#pragma once

#include <volk.h>

#include "aiko_types.h"
#include "core/utils.h"
#include "types/render_state.h"

namespace aiko::renderer::vulkan
{
    struct RenderPassCompatibilityKey
    {
        VkFormat colorFormat = VK_FORMAT_UNDEFINED;
        VkFormat depthFormat = VK_FORMAT_UNDEFINED;

        VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;

        bool operator==(const RenderPassCompatibilityKey& other) const
        {
            return colorFormat == other.colorFormat && depthFormat == other.depthFormat && samples == other.samples;
        }
    };

    struct RenderPassCompatibilityKeyHash
    {
        size_t operator()(const RenderPassCompatibilityKey& key) const
        {
            size_t seed = 0;
            utils::hashCombine(std::hash<uint32_t>{}(static_cast<uint32_t>(key.colorFormat)), seed);
            utils::hashCombine(std::hash<uint32_t>{}(static_cast<uint32_t>(key.depthFormat)), seed);
            utils::hashCombine(std::hash<uint32_t>{}(static_cast<uint32_t>(key.samples)), seed);
            return seed;
        }
    };

    struct ModelPipelineKey
    {
        RenderPassCompatibilityKey renderPass{};

        VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        AssetId shaderId = InvalidAssetId;

        FillMode fillMode = FillMode::Solid;
        CullMode cullMode = CullMode::None;

        bool depthTest = true;
        bool depthWrite = true;

        DepthCompare depthCompare = DepthCompare::LessEqual;

        bool blend = false;
        bool instanced = false;

        bool operator==(const ModelPipelineKey& other) const
        {
            return renderPass == other.renderPass
                && topology == other.topology
                && shaderId == other.shaderId
                && fillMode == other.fillMode
                && cullMode == other.cullMode
                && depthTest == other.depthTest
                && depthWrite == other.depthWrite
                && depthCompare == other.depthCompare
                && blend == other.blend
                && instanced == other.instanced;
        }
    };

    struct ModelPipelineKeyHash
    {
        size_t operator()(const ModelPipelineKey& key) const
        {
            size_t seed = 0;
            utils::hashCombine(RenderPassCompatibilityKeyHash{}(key.renderPass), seed);
            utils::hashCombine(std::hash<uint32_t>{}(static_cast<uint32_t>(key.topology)), seed);
            utils::hashCombine(std::hash<AssetId>{}(key.shaderId), seed);
            utils::hashCombine(std::hash<uint32_t>{}(static_cast<uint32_t>(key.fillMode)), seed);
            utils::hashCombine(std::hash<uint32_t>{}(static_cast<uint32_t>(key.cullMode)), seed);
            utils::hashCombine(std::hash<bool>{}(key.depthTest), seed);
            utils::hashCombine(std::hash<bool>{}(key.depthWrite), seed);
            utils::hashCombine(std::hash<uint32_t>{}(static_cast<uint32_t>(key.depthCompare)), seed);
            utils::hashCombine(std::hash<bool>{}(key.blend), seed);
            utils::hashCombine(std::hash<bool>{}(key.instanced), seed);
            return seed;
        }
    };

    struct GpuPipelineKey
    {
        RenderResourceId shaderId = InvalidRenderResourceId;
        RenderPassCompatibilityKey renderPass{};

        bool operator==(const GpuPipelineKey& other) const
        {
            return shaderId == other.shaderId && renderPass == other.renderPass;
        }
    };

    struct GpuPipelineKeyHash
    {
        size_t operator()(const GpuPipelineKey& key) const
        {
            size_t seed = 0;
            utils::hashCombine(std::hash<RenderResourceId>{}(key.shaderId), seed);
            utils::hashCombine(RenderPassCompatibilityKeyHash{}(key.renderPass), seed);
            return seed;
        }
    };

    struct GpuVertexPipelineKey
    {
        RenderResourceId shaderId = InvalidRenderResourceId;
        RenderPassCompatibilityKey renderPass{};
        VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

        bool operator==(const GpuVertexPipelineKey& other) const
        {
            return shaderId == other.shaderId
                && renderPass == other.renderPass
                && topology == other.topology;
        }
    };

    struct GpuVertexPipelineKeyHash
    {
        size_t operator()(const GpuVertexPipelineKey& key) const
        {
            size_t seed = 0;
            utils::hashCombine(std::hash<RenderResourceId>{}(key.shaderId), seed);
            utils::hashCombine(RenderPassCompatibilityKeyHash{}(key.renderPass), seed);
            utils::hashCombine(std::hash<uint32_t>{}(static_cast<uint32_t>(key.topology)), seed);
            return seed;
        }
    };
}
