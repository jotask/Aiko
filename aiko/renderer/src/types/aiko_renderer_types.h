#pragma once

#include <vector>

#include <assets/asset_id.h>

#include "core/utils.h"
#include "types/color.h"
#include "types/draw_types.h"
#include "types/render_types.h"
#include "types/transient_types.h"

namespace aiko
{
    class Texture;
    struct MeshAsset;

    struct FrameMaterialKey
    {
        AssetId shaderId = InvalidAssetId;
        AssetId diffuseTextureId = InvalidAssetId;
        const Texture* runtimeDiffuseTexture = nullptr;

        bool useVertexColor = false;
        bool lit = false;
        Color baseColor = WHITE;

        bool operator==(const FrameMaterialKey& other) const
        {
            return shaderId == other.shaderId
                && diffuseTextureId == other.diffuseTextureId
                && runtimeDiffuseTexture == other.runtimeDiffuseTexture
                && useVertexColor == other.useVertexColor
                && lit == other.lit
                && baseColor.rgba() == other.baseColor.rgba();
        }
    };

    struct FrameMaterialKeyHash
    {
        size_t operator()(const FrameMaterialKey& key) const
        {
            std::size_t seed = 0;
            utils::hashCombine(std::hash<AssetId>{}(key.shaderId), seed);
            utils::hashCombine(std::hash<AssetId>{}(key.diffuseTextureId), seed);
            utils::hashCombine(std::hash<const Texture*>{}(key.runtimeDiffuseTexture), seed);
            utils::hashCombine(std::hash<bool>{}(key.useVertexColor), seed);
            utils::hashCombine(std::hash<bool>{}(key.lit), seed);
            utils::hashCombine(std::hash<u32>{}(key.baseColor.rgba()), seed);
            return seed;
        }
    };

    struct PreparedRenderPacket
    {
        MeshDrawPacket draw;
        u64 materialId = 0;
    };

    struct PreparedInstancedPacket
    {
        InstancedDrawPacket draw;
        MaterialId materialId = 0;
        RenderResourceId meshId = 0;

        size_t mergedDataOffset = 0;
        size_t mergedByteCount = 0;
    };

    struct PreparedTransientPacket
    {
        const TransientDrawDesc* item = nullptr;
        u64 materialId = 0;
    };

    struct PreparedScenePass
    {
        vector<const GpuInstanceDrawDesc*> gpuInstances;
        vector<const GpuBillboardDrawDesc*> gpuBillboards;
        vector<const GpuVertexDrawDesc*> gpuVertices;

        vector<PreparedRenderPacket> opaque;
        vector<PreparedInstancedPacket> instanced;
        vector<PreparedTransientPacket> transient;
    };

    struct TransientCacheKey
    {
        const MeshAsset* meshAsset = nullptr;
        TransientTopology topology = TransientTopology::Triangles;

        bool operator==(const TransientCacheKey& other) const
        {
            return meshAsset == other.meshAsset && topology == other.topology;
        }
    };

    struct TransientCacheKeyHash
    {
        size_t operator()(const TransientCacheKey& key) const
        {
            std::size_t seed = 0;
            utils::hashCombine(std::hash<const MeshAsset*>{}(key.meshAsset), seed);
            utils::hashCombine(std::hash<int>{}(static_cast<int>(key.topology)), seed);
            return seed;
        }
    };
}
