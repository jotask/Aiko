#include "mesh.h"

#include "render_factory.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "constants.h"

namespace aiko
{

    Mesh::Mesh(const MeshData data)
        : Mesh()
    {
        setData(data);
    }

    Mesh::Mesh()
        : backend(nullptr)
    {
        backend=renderer::RendererFactory::createMeshImpl(this);
    }

    void Mesh::refresh()
    {
        backend->refresh();
    }

    uint Mesh::id()
    {
        return backend->id();
    }

    void Mesh::load(string filename)
    {

        auto loadMeshAsset = [] (string filename) -> Mesh::MeshData
        {

            string path = string("models/") + filename;
            auto file_path = ::aiko::global::getAssetPath(path.c_str());

            Assimp::Importer importer;
            const aiScene* pScene = importer.ReadFile(file_path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

            if (pScene == nullptr || pScene->mNumMeshes == 0)
            {
                AIKO_ASSERT(false, "Failed  to load mesh or no meshes found");
            }

            Mesh::MeshData data;

            AIKO_ASSERT(pScene->mNumMeshes == 1, "Multiple meshes not supported for now");

            // Initialize the meshes in the scene one by one
            for (uint i = 0; i < pScene->mNumMeshes; i++)
            {

                const aiMesh* paiMesh = pScene->mMeshes[i]; // For now, just take the first mesh

                // ----Vertices----
                for (unsigned int v = 0; v < paiMesh->mNumVertices; ++v)
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
                        data.m_textCoord.push_back({ uv.x, uv.y });
                    }
                    else
                    {
                        data.m_textCoord.push_back({ 0.0f, 0.0f });
                    }

                    // Optional: vertex color
                    if (paiMesh->HasVertexColors(0))
                    {
                        aiColor4D c = paiMesh->mColors[0][v];
                        data.m_colors.push_back({ c.r, c.g, c.b, c.a });
                    }
                    else
                    {
                        data.m_colors.push_back({ 1.0f, 1.0f, 1.0f, 1.0f });
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

            }

            return data;

        };

        AIKO_NOT_IMPLEMENTED;

    }

    Mesh::MeshData Mesh::getData() const
    {
        return m_data;
    }

    void Mesh::setData(const MeshData data)
    {
        m_data = data;
        refresh();
    }

    bool Mesh::isValid() const
    {
        return backend->isValid();
    }

    void Mesh::unload()
    {
        backend->unload();
    }

}
