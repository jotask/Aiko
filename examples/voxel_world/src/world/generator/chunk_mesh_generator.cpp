#include "chunk_mesh_generator.h"

#include <limits>

#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_utility.hpp>

namespace vw
{

    aiko::MeshAsset ChunkMeshGenerator::generateMeshAsset(const ChunkData& data)
    {
        aiko::MeshAsset finalAsset;
        for (std::size_t z = 0 ; z < data.size(); ++z)
        {
            for (std::size_t y = 0 ; y < data[z].size(); ++y)
            {
                for (std::size_t x = 0 ; x < data[z][y].size(); ++x)
                {
                    if (isVoxelSolid(data[z][y][x]) == false)
                    {
                        continue;
                    }
                    const aiko::ivec3 current = {static_cast<int>(x), static_cast<int>(y), static_cast<int>(z)};
                    generateVoxel(data, finalAsset, current);
                }
            }
        }

        return finalAsset;
    }

    bool ChunkMeshGenerator::isVoxelSolid(const VoxelType& voxel)
    {
        switch (voxel)
        {
        case VoxelType::AIR:
            return false;
        default:
            return true;
        }
    }

    void ChunkMeshGenerator::generateVoxel(const ChunkData& data, aiko::MeshAsset& mesh, const aiko::ivec3& current)
    {
        magic_enum::enum_for_each<FaceDirection>([&](auto val)
        {
            constexpr FaceDirection dir = val;
            if (isFaceVisible(data, dir, current ) == true)
            {
                addFace(mesh, dir, current);
            }
        });
    }

    bool ChunkMeshGenerator::isFaceVisible(const ChunkData& data, FaceDirection dir, const aiko::ivec3& current)
    {

        const aiko::ivec3 neighbourDir = generateDirFromFaceDirection(dir);

        const aiko::ivec3 neighbour = current + neighbourDir;

        auto isInChunk = [](const aiko::ivec3& neighbour) -> bool
        {
            if (neighbour.z < 0 || neighbour.z >= CHUNK_SIZE.z )
            {
                return false;
            }
            if (neighbour.y < 0 || neighbour.y >= CHUNK_SIZE.y )
            {
                return false;
            }
            if (neighbour.x < 0 || neighbour.x >= CHUNK_SIZE.x )
            {
                return false;
            }
            return true;
        };

        // If it's outside chunk, it's visible
        if (isInChunk(neighbour) == false)
        {
            return true;
        }

        // If neighbour is not solid, it's visible
        const VoxelType neighbourType = data[neighbour.z][neighbour.y][neighbour.x];

        if (isVoxelSolid(neighbourType) == false)
        {
            return true;
        }

        return false;

    }

    void ChunkMeshGenerator::addFace(aiko::MeshAsset& mesh, FaceDirection dir, const aiko::ivec3& current)
    {

        // Add 4 vertices
        const std::size_t vertexOffset = mesh.m_vertices.size();

        AIKO_ASSERT( vertexOffset + 3 <= std::numeric_limits<uint16_t>::max(), "MeshAsset uint16_t index overflow in chunk mesh");

        addFaceDirection(mesh, dir, current);

        // uvs
        mesh.m_textCoord.push_back({0.0f, 0.0f});
        mesh.m_textCoord.push_back({1.0f, 0.0f});
        mesh.m_textCoord.push_back({1.0f, 1.0f});
        mesh.m_textCoord.push_back({0.0f, 1.0f});

        // colors
        mesh.m_colors.push_back(aiko::WHITE);
        mesh.m_colors.push_back(aiko::WHITE);
        mesh.m_colors.push_back(aiko::WHITE);
        mesh.m_colors.push_back(aiko::WHITE);


        // Add 6 triangles (2 triangles)
        mesh.m_indices.push_back(vertexOffset + 0);
        mesh.m_indices.push_back(vertexOffset + 1);
        mesh.m_indices.push_back(vertexOffset + 2);
        mesh.m_indices.push_back(vertexOffset + 0);
        mesh.m_indices.push_back(vertexOffset + 2);
        mesh.m_indices.push_back(vertexOffset + 3);

    }

    void ChunkMeshGenerator::addFaceDirection(aiko::MeshAsset& asset, FaceDirection dir, const aiko::ivec3& current)
    {

        const float x0 = static_cast<float>(current.x);
        const float y0 = static_cast<float>(current.y);
        const float z0 = static_cast<float>(current.z);

        const float x1 = x0 + 1.0f;
        const float y1 = y0 + 1.0f;
        const float z1 = z0 + 1.0f;

        switch (dir)
        {
        case FaceDirection::Left:
            {
                asset.m_vertices.push_back({x0, y0, z1});
                asset.m_vertices.push_back({x0, y1, z1});
                asset.m_vertices.push_back({x0, y1, z0});
                asset.m_vertices.push_back({x0, y0, z0});
            }
            break;
        case FaceDirection::Right:
            {
                asset.m_vertices.push_back({x1, y0, z0});
                asset.m_vertices.push_back({x1, y1, z0});
                asset.m_vertices.push_back({x1, y1, z1});
                asset.m_vertices.push_back({x1, y0, z1});

            }
            break;
        case FaceDirection::Bottom:
            {
                asset.m_vertices.push_back({x0, y0, z0});
                asset.m_vertices.push_back({x1, y0, z0});
                asset.m_vertices.push_back({x1, y0, z1});
                asset.m_vertices.push_back({x0, y0, z1});
            }
            break;
        case FaceDirection::Top:
            {
                asset.m_vertices.push_back({x0, y1, z0});
                asset.m_vertices.push_back({x0, y1, z1});
                asset.m_vertices.push_back({x1, y1, z1});
                asset.m_vertices.push_back({x1, y1, z0});
            }
            break;
        case FaceDirection::Back:
            {
                asset.m_vertices.push_back({x0, y0, z0});
                asset.m_vertices.push_back({x0, y1, z0});
                asset.m_vertices.push_back({x1, y1, z0});
                asset.m_vertices.push_back({x1, y0, z0});
            }
            break;
        case FaceDirection::Front:
            {
                asset.m_vertices.push_back({x0, y0, z1});
                asset.m_vertices.push_back({x1, y0, z1});
                asset.m_vertices.push_back({x1, y1, z1});
                asset.m_vertices.push_back({x0, y1, z1});
            }
            break;
        }

        const aiko::ivec3 normal = generateDirFromFaceDirection(dir);
        for (int i = 0; i < 4; ++i)
        {
            asset.m_normals.push_back({
                static_cast<float>(normal.x),
                static_cast<float>(normal.y),
                static_cast<float>(normal.z)
            });
        }

    }

    aiko::ivec3 ChunkMeshGenerator::generateDirFromFaceDirection(FaceDirection dir)
    {
        switch (dir)
        {
            case FaceDirection::Left:   return {-1,  0,  0};
            case FaceDirection::Right:  return { 1,  0,  0};
            case FaceDirection::Bottom: return { 0, -1,  0};
            case FaceDirection::Top:    return { 0,  1,  0};
            case FaceDirection::Back:   return { 0,  0, -1};
            case FaceDirection::Front:  return { 0,  0,  1};
        }
        return { 0 };
    }
}

