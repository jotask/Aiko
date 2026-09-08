#include "asset_importer.h"

#include "constants.h"

#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace aiko
{

    TextureAsset AssetImporter::loadTexture(const string& file_path)
    {

        std::string base = global::GLOBAL_ASSET_PATH;
        base += std::string(file_path);

        AIKO_ASSERT(std::filesystem::exists(base), "Texture don't exist");

        int width;
        int height;
        int channels;

        unsigned char* data = stbi_load(base.c_str(), &width, &height, &channels, 4); // force RGBA

        AIKO_ASSERT(data, "Texture Failed to load texture.")

        TextureAsset asset = {};
        asset.desc.type = TextureType::Sampled;
        asset.desc.format = TextureFormat::RGBA8;
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
                        data[sourceOffset + 0],
                        data[sourceOffset + 1],
                        data[sourceOffset + 2],
                        data[sourceOffset + 3]
                    );
            }
        }

        stbi_image_free(data);

        return asset;

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

    MeshAsset AssetImporter::loadMesh(const string& file_path)
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
        AIKO_ASSERT(scene != nullptr && scene->mNumMeshes > 0, "Failed to load mesh");

        const aiMesh* paiMesh = scene->mMeshes[0];

        return importAiMesh(paiMesh);

    }

    ImportedModel AssetImporter::loadModel(const string& filePath)
    {

        string path = string("models/") + filePath;
        auto finalFilePath = global::getAssetPath(path.c_str());

        Assimp::Importer importer;

        const uint post =
            aiProcess_Triangulate |
            aiProcess_GenNormals |
            aiProcess_FlipUVs |
            aiProcess_JoinIdenticalVertices |
            aiProcess_FlipWindingOrder;

        const aiScene* scene = importer.ReadFile(finalFilePath, post);

        AIKO_ASSERT(scene != nullptr && scene->mNumMeshes > 0, "Failed to load model");

        ImportedModel result{};

        for (uint i = 0; i < scene->mNumMeshes; ++i)
        {
            const aiMesh* aiMesh = scene->mMeshes[i];
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

        return result;

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
