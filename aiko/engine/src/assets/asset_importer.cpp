#include "asset_importer.h"

#include "assets/import/skinned_mesh_importer.h"
#include "animation/skeleton_pose.h"
#include "math/math_transform.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/config.h>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <filesystem>
#include <memory>
#include <cmath>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace aiko
{

    AssetImportResult<TextureAsset> AssetImporter::loadTexture(const std::filesystem::path& file_path, TextureColorSpace colorSpace)
    {

        const std::string base = file_path.string();

        int width = 0;
        int height = 0;
        int channels = 0;

        std::unique_ptr<stbi_uc, decltype(&stbi_image_free)> data(
            stbi_load(base.c_str(), &width, &height, &channels, 4),
            &stbi_image_free
        );

        if (!data)
        {
            const char* reason = stbi_failure_reason();

            std::error_code fsError;
            const bool exists = std::filesystem::exists(base, fsError);

            if (!fsError && !exists)
            {
                return AssetImportResult<TextureAsset>::failure(AssetImportError::FileNotFound, "Failed to load texture '" + base + "': " + (reason != nullptr ? reason : "Unknown decoding error"));
            }
            else
            {
                return AssetImportResult<TextureAsset>::failure(AssetImportError::DecodeFailed, "Failed to load texture '" + base + "': " + (reason != nullptr ? reason : "Unknown decoding error"));
            }

        }

        if (width <= 0 || height <= 0)
        {
            return AssetImportResult<TextureAsset>::failure(AssetImportError::InvalidData, "Texture has invalid dimensions: " + base);
        }

        TextureAsset asset = {};
        asset.desc.type = TextureType::Sampled;
        switch (colorSpace)
        {
            case TextureColorSpace::Linear:
                asset.desc.format = TextureFormat::RGBA8;
                break;

            case TextureColorSpace::SRGB:
                asset.desc.format = TextureFormat::RGBA8_SRGB;
                break;

            default:
                return AssetImportResult<TextureAsset>::failure(AssetImportError::InvalidData, "Invalid texture color space: " + file_path.string());
        }
        asset.desc.width = width;
        asset.desc.height = height;
        asset.desc.mipmaps = 1;
        asset.desc.computeWrite = false;

        const size_t pixelCount = static_cast<size_t>(width) * static_cast<size_t>(height);
        asset.pixels.resize(pixelCount);

        for (size_t y = 0; y < static_cast<size_t>(height); ++y)
        {
            const size_t sourceY = static_cast<size_t>(height) - 1 - y;
            for (size_t x = 0; x < static_cast<size_t>(width); ++x)
            {
                const size_t sourceOffset = (sourceY * static_cast<size_t>(width) + x) * 4;
                const size_t targetOffset = y * static_cast<size_t>(width) + x;
                asset.pixels[targetOffset] =
                    Color::fromBytes(
                        data.get()[sourceOffset + 0],
                        data.get()[sourceOffset + 1],
                        data.get()[sourceOffset + 2],
                        data.get()[sourceOffset + 3]
                    );
            }
        }

        return AssetImportResult<TextureAsset>::success(std::move(asset));

    }

    static MeshAsset importAiMesh(const aiMesh* paiMesh)
    {
        MeshAsset data{};

        for (uint v = 0; v < paiMesh->mNumVertices; ++v)
        {
            const aiVector3D vert = paiMesh->mVertices[v];
            data.m_vertices.push_back({ vert.x, vert.y, vert.z });

            if (paiMesh->HasNormals() == true)
            {
                const aiVector3D n = paiMesh->mNormals[v];
                data.m_normals.push_back({ n.x, n.y, n.z });
            }
            else
            {
                data.m_normals.push_back({ 0.0f, 0.0f, 0.0f });
            }

            if (paiMesh->HasTextureCoords(0) == true)
            {
                const aiVector3D uv = paiMesh->mTextureCoords[0][v];
                data.m_textCoord.push_back({ uv.x, -uv.y });
            }
            else
            {
                data.m_textCoord.push_back({ 0.0f, 0.0f });
            }

            if (paiMesh->HasVertexColors(0) == true)
            {
                const aiColor4D c = paiMesh->mColors[0][v];
                data.m_colors.push_back({ c.r, c.g, c.b, c.a });
            }
            else
            {
                data.m_colors.push_back(WHITE);
            }
        }

        for (uint f = 0; f < paiMesh->mNumFaces; ++f)
        {
            const aiFace& face = paiMesh->mFaces[f];
            for (uint j = 0; j < face.mNumIndices; ++j)
            {
                data.m_indices.push_back(face.mIndices[j]);
            }
        }

        return data;

    }

    static std::string importAiMorphTargets(const aiMesh& source, MeshAsset& destination)
    {
        if (source.mNumAnimMeshes == 0)
        {
            return {};
        }

        if (source.mAnimMeshes == nullptr || source.mVertices == nullptr || destination.m_vertices.size() != source.mNumVertices)
        {
            return "Invalid morph target source geometry";
        }

        std::unordered_set<std::string> names;
        destination.m_morphTargets.reserve(source.mNumAnimMeshes);

        for (unsigned int i = 0; i < source.mNumAnimMeshes; ++i)
        {
            const aiAnimMesh* anim = source.mAnimMeshes[i];

            if (anim == nullptr || anim->mNumVertices != source.mNumVertices)
            {
                return "Invalid morph target at index " + std::to_string(i);
            }

            MorphTargetAsset target{};
            target.name = anim->mName.C_Str();

            if (target.name.empty())
            {
                target.name = "target_" + std::to_string(i);
            }

            if (!names.insert(target.name).second)
            {
                return "Duplicate morph target '" + target.name + "'";
            }

            // Every target has one position delta per base vertex.
            // Missing position overrides mean zero displacement.
            target.positionDeltas.resize(source.mNumVertices);

            if (anim->HasPositions())
            {
                for (unsigned int v = 0; v < source.mNumVertices; ++v)
                {
                    const aiVector3D delta = anim->mVertices[v] - source.mVertices[v];

                    const vec3 positionDelta = { delta.x, delta.y, delta.z };

                    if (aiko::math::isfinite(positionDelta) == false)
                    {
                        return "Non-finite position delta in '" + target.name + "'";
                    }

                    target.positionDeltas[v] = positionDelta;
                }
            }

            // Morph normals are optional.
            if (anim->HasNormals())
            {
                if (!source.HasNormals())
                {
                    return "Morph target has normals but base mesh does not";
                }

                target.normalDeltas.resize(source.mNumVertices);

                for (unsigned int v = 0; v < source.mNumVertices; ++v)
                {
                    const aiVector3D delta = anim->mNormals[v] - source.mNormals[v];

                    const vec3 normalDelta = { delta.x, delta.y, delta.z };

                    if (aiko::math::isfinite(normalDelta) == false)
                    {
                        return "Non-finite normal delta in '" + target.name + "'";
                    }

                    target.normalDeltas[v] = normalDelta;
                }
            }

            destination.m_morphTargets.push_back(std::move(target));
        }

        return {};
    }

    static AssetImportResult<ImportedModel> importSceneMeshes(const aiScene& scene, ImportedModel result)
    {
        result.submeshes.reserve(scene.mNumMeshes);

        for (unsigned int i = 0; i < scene.mNumMeshes; ++i)
        {
            const aiMesh* mesh = scene.mMeshes[i];

            if (mesh == nullptr || mesh->mMaterialIndex >= scene.mNumMaterials || scene.mMaterials[mesh->mMaterialIndex] == nullptr)
            {
                return AssetImportResult<ImportedModel>::failure(AssetImportError::InvalidData, "Model contains an invalid mesh or material");
            }

            const aiMaterial* material = scene.mMaterials[mesh->mMaterialIndex];

            ImportedModelSubMesh submesh{};

            submesh.mesh = importAiMesh(mesh);

            const std::string morphError = importAiMorphTargets(*mesh, submesh.mesh);

            if (!morphError.empty())
            {
                return AssetImportResult<ImportedModel>::failure(AssetImportError::InvalidData, "Mesh " + std::to_string(i) + ": " + morphError);
            }

            submesh.useVertexColor = mesh->HasVertexColors(0);

            submesh.lit = true;
            submesh.shaderSource = "model";

            aiString texturePath;

            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
            {
                submesh.diffuseTextureSource = texturePath.C_Str();
            }

            aiColor3D diffuse(1.0f, 1.0f, 1.0f);

            material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);

            submesh.baseColor =
            {
                diffuse.r,
                diffuse.g,
                diffuse.b,
                1.0f
            };

            if (submesh.useVertexColor && submesh.diffuseTextureSource.empty())
            {
                submesh.baseColor = WHITE;
            }

            result.submeshes.push_back(std::move(submesh));
        }

        return AssetImportResult<ImportedModel>::success(std::move(result));
    }

    AssetImportResult<MeshAsset> AssetImporter::loadMesh(const std::filesystem::path& file_path)
    {
        const std::string final_file_path = file_path.string();

        Assimp::Importer importer;

        const uint post =
            aiProcess_Triangulate |
            aiProcess_GenNormals |
            aiProcess_FlipUVs |
            aiProcess_JoinIdenticalVertices |
            aiProcess_FlipWindingOrder;

        const aiScene* scene = importer.ReadFile(final_file_path, post);

        if (scene == nullptr || scene->mNumMeshes == 0 || scene->mMeshes == nullptr || scene->mMeshes[0] == nullptr)
        {
            return AssetImportResult<MeshAsset>::failure(AssetImportError::DecodeFailed, "Failed to load mesh '" + final_file_path + "': " + importer.GetErrorString());
        }

        const aiMesh* paiMesh = scene->mMeshes[0];

        MeshAsset mesh = importAiMesh(paiMesh);

        const std::string morphError = importAiMorphTargets(*paiMesh, mesh);

        if (!morphError.empty())
        {
            return AssetImportResult<MeshAsset>::failure(AssetImportError::InvalidData, morphError);
        }

        return AssetImportResult<MeshAsset>::success(std::move(mesh));

    }

    AssetImportResult<ImportedModel> AssetImporter::loadModel(const std::filesystem::path& filePath)
    {

        const std::string finalFilePath = filePath.string();

        Assimp::Importer importer;

        importer.SetPropertyBool(AI_CONFIG_PP_PTV_KEEP_HIERARCHY, true);

        const unsigned int post =
            aiProcess_Triangulate |
            aiProcess_GenNormals |
            aiProcess_FlipUVs |
            aiProcess_JoinIdenticalVertices |
            aiProcess_FlipWindingOrder;

        const aiScene* scene = importer.ReadFile(finalFilePath, post);

        if (scene == nullptr || scene->mNumMeshes == 0 || scene->mMeshes == nullptr || scene->mMaterials == nullptr)
        {
            return AssetImportResult<ImportedModel>::failure(AssetImportError::DecodeFailed, "Failed to load model '" + finalFilePath + "': " + importer.GetErrorString());
        }

        for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
        {
            if (scene->mMeshes[i] == nullptr)
            {
                return AssetImportResult<ImportedModel>::failure(AssetImportError::InvalidData, "Model contains a null mesh: " + finalFilePath);
            }
        }

        const bool skinned = SkinnedMeshImporter::hasSkinnedMeshes(*scene);

        if (!skinned)
        {
            // Static models retain their existing import behavior.
            scene = importer.ApplyPostProcessing(aiProcess_PreTransformVertices);
            if (scene == nullptr)
            {
                return AssetImportResult<ImportedModel>::failure(AssetImportError::DecodeFailed, "Failed to preprocess static model: " + finalFilePath);
            }
        }

        // Import common geometry and materials.
        auto imported = importSceneMeshes(*scene, ImportedModel{});
        if (!imported)
        {
            return imported;
        }

        if (skinned)
        {
            return SkinnedMeshImporter::importSkinnedModel(*scene, std::move(*imported));
        }

        return imported;
    }

    ShaderAsset AssetImporter::loadShader(const string& path)
    {
        const ShaderAsset asset
        {
            .vertexPath = path + ".vs",
            .fragmentPath = path + ".fs"
        };
        return asset;
    }

    ComputeShaderAsset AssetImporter::loadComputeShader(const string& path)
    {
        const ComputeShaderAsset asset
        {
            .computePath = path + ".cs"
        };
        return asset;
    }
}
