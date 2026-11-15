#pragma once

#include "assets/asset_import_result.h"
#include "assets/types/mesh_asset.h"
#include "assets/types/skeleton_asset.h"
#include "model_import_result.h"

#include <assimp/Importer.hpp>
#include <assimp/config.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <types/color.h>

#include <aiko_types.h>
#include <optional>
#include <unordered_set>

namespace aiko
{
    class SkinnedMeshImporter final
    {
    public:
        static AssetImportResult<ImportedModel> importSkinnedModel(const aiScene& scene, ImportedModel imported);
        static bool hasSkinnedMeshes(const aiScene& scene);
    private:
        struct VertexInfluence
        {
            uint32_t jointIndex = 0;
            float weight = 0.0f;
        };

        static mat4 toAikoMatrix(const aiMatrix4x4& source);
        static AssetImportResult<SkeletonAsset> buildSkeleton(const aiScene& scene);
        static void appendSkeletonJoints(const aiNode& node, int32_t parentIndex, const std::unordered_set<const aiNode*>& requiredNodes, SkeletonAsset& skeleton);
        static std::string rebuildBindPoseFromOffsets(const aiScene& scene, const std::vector<mat4>& meshBindTransforms, const std::unordered_map<std::string, uint32_t>& jointLookup, SkeletonAsset& skeleton);
        static bool isFiniteMatrix(const mat4& matrix);
        static bool matricesNearlyEqual(const mat4& a, const mat4& b);
        static std::string importVertexWeights(const aiMesh& source, const std::unordered_map<std::string, uint32_t>& jointLookup, MeshAsset& destination);
        static std::string collectMeshBindTransforms(const aiScene& scene, std::vector<mat4>& transforms);
        static AssetImportResult<std::vector<AnimationClipAsset>> importAnimations(const aiScene& scene, const SkeletonAsset& skeleton);
    };
}
