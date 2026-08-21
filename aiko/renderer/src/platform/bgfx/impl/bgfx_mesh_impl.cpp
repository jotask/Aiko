#include "bgfx_mesh_impl.h"

#include "models/mesh.h"
#include "platform/bgfx/bgfx_types.h"

namespace aiko::renderer::bgfx
{
    namespace shared {
        struct VertexInformation;
    }

    BgfxMeshImpl::BgfxMeshImpl(const Mesh* mesh)
        : IMeshImpl(mesh)
        , m_vertexBuffer(AIKO_INVALID_HANDLE)
        , m_indexBuffer(AIKO_INVALID_HANDLE)
    {

    }

    bool BgfxMeshImpl::isValid() const
    {
        return ::bgfx::isValid(m_vertexBuffer) && ::bgfx::isValid(m_indexBuffer);
    }

    void BgfxMeshImpl::unload()
    {
        ::bgfx::destroy(m_vertexBuffer);
        ::bgfx::destroy(m_indexBuffer);
    }

    void BgfxMeshImpl::refresh(const MeshAsset& asset)
    {

        // Vertex buffer
        {
            const auto vertices = convertToVBH(asset);
            AIKO_ASSERT(vertices.empty() == false , "No vertices");
            const ::bgfx::Memory* memV = ::bgfx::copy(vertices.data(), static_cast<uint32_t>(vertices.size() * sizeof(VertexInformation)));
            if (::bgfx::isValid(m_vertexBuffer) == true)
            {
                ::bgfx::update(m_vertexBuffer, 0, memV);
            }
            else
            {
                m_vertexBuffer = ::bgfx::createDynamicVertexBuffer(memV, s_global_layout);
            }
            AIKO_ASSERT(::bgfx::isValid(m_vertexBuffer), "Invalid Vertex Buffer");
        }

        // Indices buffer
        {
            const auto indices = convertToIBH(asset);
            AIKO_ASSERT(indices.empty() == false , "No indices");
            const ::bgfx::Memory* memI = ::bgfx::copy(indices.data(), static_cast<uint32_t>(indices.size() * sizeof(uint16_t)));
            if (::bgfx::isValid(m_indexBuffer) == true)
            {
                ::bgfx::update(m_indexBuffer, 0, memI);
            }
            else
            {
                m_indexBuffer = ::bgfx::createDynamicIndexBuffer(memI);
            }
            AIKO_ASSERT(::bgfx::isValid(m_indexBuffer), "Invalid Index Buffer");
        }

    }

    RenderResourceId BgfxMeshImpl::id()
    {
        const auto vbIDX = getVertexBuffferHandler().idx;
        const auto ibIDX = getIndexBuffferHandler().idx;
        return static_cast<uint>(vbIDX) << 16 | ibIDX;
    }

    vector<VertexInformation> BgfxMeshImpl::convertToVBH(const MeshAsset& asset)
    {

        vector<VertexInformation> vertices;

        for (size_t i = 0; i < asset.m_vertices.size(); ++i)
        {
            VertexInformation v{0};

            // position
            const auto vertex = asset.m_vertices[i];
            v.x = vertex.x;
            v.y = vertex.y;
            v.z = vertex.z;

            // uv
            const auto textCoord = asset.m_textCoord[i];
            v.u = textCoord.x;
            v.v = textCoord.y;

            // color
            v.abgr = asset.m_colors[i].rgba();

            // Normal
            const auto normal = asset.m_normals[i];
            v.n_x = normal.x;
            v.n_y = normal.y;
            v.n_z = normal.z;

            vertices.push_back(v);
        }

        return vertices;

    }

    vector<uint16_t> BgfxMeshImpl::convertToIBH(const MeshAsset& asset)
    {
        vector<uint16_t> indices16;
        indices16.reserve(asset.m_indices.size());
        for (uint32_t idx : asset.m_indices)
        {
            indices16.push_back(static_cast<uint16_t>(idx));
        }
        return indices16;
    }
}
