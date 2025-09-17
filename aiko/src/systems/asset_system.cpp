#include "asset_system.h"

#include <algorithm>

#include "modules/module_connector.h"
#include "systems/system_connector.h"

#include "modules/render/render_module.h"
#include "components/transform_component.h"
#include "components/camera_component.h"
#include "types/camera_types.h"
#include "models/camera.h"
#include "models/shader.h"
#include "constants.h"
#include "systems/render_system.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace aiko
{

    void AssetSystem::unload(asset::Asset* asset)
    {
        this->unload(asset->getID());
    }

    void AssetSystem::unload(asset::ID asset)
    {
        auto it = std::find_if(m_assets.begin(), m_assets.end(), [&](const AikoPtr<asset::Asset>& ptr) {
            return ptr->getID() == asset;
            });
        if (it != m_assets.end())
        {
            (*it)->unload();
            (*it).reset(); // Call destructor
            m_assets.erase(it);
        }
    }

    Mesh AssetSystem::loadMesh(const char* filename)
    {

        string path = string("models/") + filename;
        auto file_path = ::aiko::global::getAssetPath(path.c_str());

        Assimp::Importer importer;
        const aiScene* pScene = importer.ReadFile(file_path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

        if (pScene == nullptr || pScene->mNumMeshes == 0)
        {
            assert(false && "Failed  to load mesh or no meshes found");
        }

        Mesh mesh;
        
        assert(pScene->mNumMeshes == 1 && "Multiple meshes not supported for now");

        // Initialize the meshes in the scene one by one
        for (uint i = 0; i < pScene->mNumMeshes; i++)
        {

            const aiMesh* paiMesh = pScene->mMeshes[i]; // For now, just take the first mesh

            // ----Vertices----
            for (unsigned int v = 0; v < paiMesh->mNumVertices; ++v)
            {
                aiVector3D vert = paiMesh->mVertices[v];
                mesh.m_vertices.push_back({ vert.x, vert.y, vert.z });

                // Normals
                if (paiMesh->HasNormals())
                {
                    aiVector3D n = paiMesh->mNormals[v];
                    mesh.m_normals.push_back({ n.x, n.y, n.z });
                }
                else
                {
                    mesh.m_normals.push_back({ 0.0f, 0.0f, 0.0f });
                }

                // Texture coordinates
                if (paiMesh->HasTextureCoords(0))
                {
                    aiVector3D uv = paiMesh->mTextureCoords[0][v];
                    mesh.m_teexCoord.push_back({ uv.x, uv.y });
                }
                else
                {
                    mesh.m_teexCoord.push_back({ 0.0f, 0.0f });
                }

                // Optional: vertex color
                if (paiMesh->HasVertexColors(0))
                {
                    aiColor4D c = paiMesh->mColors[0][v];
                    mesh.m_colors.push_back({ c.r, c.g, c.b, c.a });
                }
                else
                {
                    mesh.m_colors.push_back({ 1.0f, 1.0f, 1.0f, 1.0f });
                }

            }

            // ---- Indices ----
            for (unsigned int f = 0; f < paiMesh->mNumFaces; ++f)
            {
                const aiFace& face = paiMesh->mFaces[f];
                for (unsigned int i = 0; i < face.mNumIndices; ++i)
                {
                    mesh.m_indices.push_back(face.mIndices[i]);
                }
            }
        
        }

        return mesh;
        
    }

    Model AssetSystem::loadModel(const char* filename)
    {

        string path = string("models/") + filename;
        auto file_path = ::aiko::global::getAssetPath(path.c_str());

        Assimp::Importer importer;
        const aiScene* pScene = importer.ReadFile(file_path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

        if (pScene == nullptr || pScene->mNumMeshes == 0)
        {
            assert(false && "Failed  to load mesh or no meshes found");
        }

        Model model;

        assert(pScene->mNumMeshes == 1 && "Multiple meshes not supported for now");

        // Initialize the meshes in the scene one by one
        for (uint i = 0; i < pScene->mNumMeshes; i++)
        {

            const aiMesh* paiMesh = pScene->mMeshes[i]; // For now, just take the first mesh

            // ----Vertices----
            for (unsigned int v = 0; v < paiMesh->mNumVertices; ++v)
            {
                aiVector3D vert = paiMesh->mVertices[v];
                model.m_mesh.m_vertices.push_back({ vert.x, vert.y, vert.z });

                // Normals
                if (paiMesh->HasNormals())
                {
                    aiVector3D n = paiMesh->mNormals[v];
                    model.m_mesh.m_normals.push_back({ n.x, n.y, n.z });
                }
                else
                {
                    model.m_mesh.m_normals.push_back({ 0.0f, 0.0f, 0.0f });
                }

                // Texture coordinates
                if (paiMesh->HasTextureCoords(0))
                {
                    aiVector3D uv = paiMesh->mTextureCoords[0][v];
                    model.m_mesh.m_teexCoord.push_back({ uv.x, - uv.y });
                }
                else
                {
                    model.m_mesh.m_teexCoord.push_back({ 0.0f, 0.0f });
                }

                // Optional: vertex color
                if (paiMesh->HasVertexColors(0))
                {
                    aiColor4D c = paiMesh->mColors[0][v];
                    model.m_mesh.m_colors.push_back({ c.r, c.g, c.b, c.a });
                }
                else
                {
                    model.m_mesh.m_colors.push_back({ 1.0f, 1.0f, 1.0f, 1.0f });
                }

            }

            // ---- Indices ----
            for (unsigned int f = 0; f < paiMesh->mNumFaces; ++f)
            {
                const aiFace& face = paiMesh->mFaces[f];
                for (unsigned int i = 0; i < face.mNumIndices; ++i)
                {
                    model.m_mesh.m_indices.push_back(face.mIndices[i]);
                }
            }

            const aiMaterial* material = pScene->mMaterials[paiMesh->mMaterialIndex];

            aiString texturePath;
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
            {
                std::string texFile = string("models/") + texturePath.C_Str();
                model.m_material.m_diffuse.loadTextureFromFile(texFile.c_str());
            }

        }

		model.m_mesh.refresh();

        return model;

    }

    void AssetSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_SYSTEM_REQUIRED(RenderSystem, systemConnector, m_renderSystem)
    }
    
    void AssetSystem::init()
    {
        Model::s_assetSystem = this;
    }
    
    void AssetSystem::update()
    {
    
    }
    
    void AssetSystem::render()
    {

    }

}
