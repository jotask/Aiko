#pragma once

#include "animation_clip_asset.h"
#include "material_asset.h"
#include "skeleton_asset.h"

#include <assets/asset_id.h>
#include <math/math_vector.h>

#include <aiko_types.h>
#include <optional>
#include <string>
#include <vector>

namespace aiko
{

    struct MorphTargetAsset
    {
        std::string name;
        std::vector<vec3> positionDeltas;
        std::vector<vec3> normalDeltas;
    };

    struct MeshAsset
    {
        vector<vec3>        m_vertices;
        vector<vec2>        m_textCoord;
        vector<vec3>        m_normals;
        vector<Color>       m_colors;
        vector<vec4>        m_tangents;
        vector<uint32_t>    m_indices;

	    std::vector<uvec4>  m_jointIndices;
	    std::vector<vec4>   m_jointWeights;

        std::vector<MorphTargetAsset> m_morphTargets;
    };

    struct ModelAsset
    {
        struct SubMesh
        {
            AssetId meshId = InvalidAssetId;
            MaterialAsset material;
        };
        vector<SubMesh> submeshes;
        std::optional<SkeletonAsset> skeleton;
        std::vector<AnimationClipAsset> animations;
    };

}