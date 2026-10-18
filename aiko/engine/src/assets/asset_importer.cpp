#include "asset_importer.h"

#include "constants.h"

#include <filesystem>
#include <memory>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace aiko
{

    AssetImportResult<TextureAsset> AssetImporter::loadTexture(const string& file_path, TextureColorSpace colorSpace)
    {

        std::string base = global::GLOBAL_ASSET_PATH;
        base += std::string(file_path);

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
                return AssetImportResult<TextureAsset>::failure(
                    AssetImportError::InvalidData,
                    "Invalid texture color space: " + file_path
                );
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

    AssetImportResult<MeshAsset> AssetImporter::loadMesh(const string& file_path)
    {
        auto final_file_path = global::getAssetPath(file_path.c_str());

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

        return AssetImportResult<MeshAsset>::success(importAiMesh(paiMesh));

    }

    AssetImportResult<ImportedModel> AssetImporter::loadModel(const string& filePath)
    {

        const string finalFilePath = global::getAssetPath(filePath.c_str());

        Assimp::Importer importer;

        const uint post =
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

        ImportedModel result{};

        for (uint i = 0; i < scene->mNumMeshes; ++i)
        {
            const aiMesh* aiMesh = scene->mMeshes[i];

            if (aiMesh == nullptr || aiMesh->mMaterialIndex >= scene->mNumMaterials || scene->mMaterials[aiMesh->mMaterialIndex] == nullptr)
            {
                return AssetImportResult<ImportedModel>::failure(AssetImportError::InvalidData, "Model contains an invalid mesh or material: " + finalFilePath);
            }

            const aiMaterial* aiMaterial = scene->mMaterials[aiMesh->mMaterialIndex];

            ImportedModelSubMesh submesh{};

            submesh.mesh = importAiMesh(aiMesh);
            submesh.useVertexColor = aiMesh->HasVertexColors(0);
            submesh.lit = true;
            submesh.shaderSource = "model";

            aiString texturePath;

            if (aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
            {
                submesh.diffuseTextureSource = string("models/") + texturePath.C_Str();
            }

            aiColor3D diffuse(1.0f, 1.0f, 1.0f);

            if (AI_SUCCESS != aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse))
            {
                diffuse = aiColor3D(1.0f, 1.0f, 1.0f);
            }

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
