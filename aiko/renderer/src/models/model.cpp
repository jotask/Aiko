#include "model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "constants.h"

namespace aiko
{

    AssetSystem* Model::s_assetSystem = nullptr;

    Model::Model()
    {

    }

    void Model::load(const char* filename)
    {

        auto loadAssetModel = [](const char* filename) -> std::vector<MeshMatData>
        {
            string path = string("models/") + filename;
            auto file_path = ::aiko::global::getAssetPath(path.c_str());

            Assimp::Importer importer;

            uint post = aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_FlipWindingOrder;

            const aiScene* pScene = importer.ReadFile(file_path, post);

            if (pScene == nullptr || pScene->mNumMeshes == 0)
            {
                AIKO_ASSERT(false, "Failed  to load mesh or no meshes found");
            }

            std::vector<Model::MeshMatData> models;

            // Initialize the meshes in the scene one by one
            for (uint i = 0; i < pScene->mNumMeshes; i++)
            {

                Model::MeshMatData model;
                Mesh::MeshData data;

                const aiMesh* paiMesh = pScene->mMeshes[i]; // For now, just take the first mesh

                // ----Vertices----
                for (uint v = 0; v < paiMesh->mNumVertices; ++v)
                {

                    aiVector3D vert = paiMesh->mVertices[v];
                    data.m_vertices.push_back({ vert.x, vert.y, vert.z });

                    // Normals
                    if (paiMesh->HasNormals())
                    {
                        aiVector3D n = paiMesh->mNormals[v];
                        data.m_normals.push_back({ n.x, n.y, n.z });
                    }
                    else
                    {
                        data.m_normals.push_back({ 0.0f, 0.0f, 0.0f });
                    }

                    // Texture coordinates
                    if (paiMesh->HasTextureCoords(0))
                    {
                        aiVector3D uv = paiMesh->mTextureCoords[0][v];
                        data.m_textCoord.push_back({ uv.x, - uv.y });
                    }
                    else
                    {
                        data.m_textCoord.push_back({ 0.0f, 0.0f });
                    }

                    // Vertex color
                    if (paiMesh->HasVertexColors(0))
                    {
                        aiColor4D c = paiMesh->mColors[0][v];
                        data.m_colors.push_back({ c.r, c.g, c.b, c.a });
                    }
                    else
                    {
                        data.m_colors.push_back(MAGENTA);
                    }

                }

                // ---- Indices ----
                for (unsigned int f = 0; f < paiMesh->mNumFaces; ++f)
                {
                    const aiFace& face = paiMesh->mFaces[f];
                    for (unsigned int i = 0; i < face.mNumIndices; ++i)
                    {
                        data.m_indices.push_back(face.mIndices[i]);
                    }
                }

                model.mesh.setData(data);

                const aiMaterial* material = pScene->mMaterials[paiMesh->mMaterialIndex];

                aiString texturePath;
                if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
                {
                    std::string texFile = string("models/") + texturePath.C_Str();
                    model.material.m_diffuse.load(texFile);
                }

                aiColor3D diffuse(1.0f, 1.0f, 1.0f); // default white
                if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse))
                {
                    diffuse = aiColor3D(1.0f, 1.0f, 1.0f); // fallback
                }

			    model.material.m_baseColor = { diffuse.r, diffuse.g, diffuse.b, 1.0f };

		        model.mesh.refresh();
			    models.push_back(model);

            }

            return models;
        };

        m_meshes = loadAssetModel(filename);
        for (auto& mesh : m_meshes)
        {
            mesh.material.m_shader.load("model");
        }
    }

}
