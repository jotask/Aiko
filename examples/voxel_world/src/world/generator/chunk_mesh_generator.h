#pragma once

#include "assets/types/mesh_asset.h"
#include "world/types/chunk_types.h"

namespace vw
{

    class ChunkMeshGenerator
    {
    public:
        static aiko::MeshAsset generateMeshAsset(const ChunkData& data);
    private:

        enum class FaceDirection
        {
            Left,
            Right,
            Bottom,
            Top,
            Back,
            Front,
        };

        static bool isVoxelSolid(const VoxelType& voxel);
        static void generateVoxel(const ChunkData& data, aiko::MeshAsset& mesh, const aiko::ivec3& current);

        static bool isFaceVisible(const ChunkData& data, FaceDirection dir, const aiko::ivec3& current );
        static void addFace(aiko::MeshAsset& mesh, FaceDirection dir, const aiko::ivec3& current);
        static void addFaceDirection(aiko::MeshAsset& asset, FaceDirection dir, const aiko::ivec3& current);

        static aiko::ivec3 generateDirFromFaceDirection(FaceDirection dir);

    };

}

