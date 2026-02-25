#include "bgfx_mesh_impl.h"

#include <core/file.h>

#include "models/mesh.h"
#include "platform/bgfx/bgfx_platform_helper.h"
#include "platform/bgfx/bgfx_types.h"

namespace aiko::renderer::bgfx
{
    namespace shared {
        struct VertexInformation;
    }

    BgfxMeshImpl::BgfxMeshImpl(const Mesh* mesh)
        : IMeshImpl(mesh)
        , m_vertexBuffer({::bgfx::kInvalidHandle})
        , m_indexBuffer({::bgfx::kInvalidHandle})
    {
        m_texture.create(1, 1);
        std::vector<Color> pixels = {AIKO_DEFAULT_MESH_COLOR};
        m_texture.setPixels(pixels);
        AIKO_ASSERT(m_texture.isValid(), "Invalid texture creation")
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

    void BgfxMeshImpl::refresh()
    {

        // Vertex buffer
        {
            const auto vertices = convertToVBH();
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
            const auto indices = convertToIBH();
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

    uint BgfxMeshImpl::id()
    {
        const auto vbIDX = getVertexBuffferHandler().idx;
        const auto ibIDX = getIndexBuffferHandler().idx;
        return static_cast<uint>(vbIDX) << 16 | ibIDX;
    }

    std::vector<VertexInformation> BgfxMeshImpl::convertToVBH()
    {

        const Mesh::MeshData data = m_mesh->getData();
        std::vector<VertexInformation> vertices;

        for (size_t i = 0; i < data.m_vertices.size(); ++i)
        {
            VertexInformation v{0};

            // position
            const auto vertex = data.m_vertices[i];
            v.x = vertex.x;
            v.y = vertex.y;
            v.z = vertex.z;

            // uv
            const auto textCoord = data.m_textCoord[i];
            v.u = textCoord.x;
            v.v = textCoord.y;

            // color
            v.abgr = data.m_colors[i].rgba();

            // Normal
            const auto normal = data.m_normals[i];
            v.n_x = normal.x;
            v.n_y = normal.y;
            v.n_z = normal.z;

            vertices.push_back(v);
        }

        return vertices;

    }

    std::vector<uint16_t> BgfxMeshImpl::convertToIBH()
    {

        const Mesh::MeshData data = m_mesh->getData();

        std::vector<uint16_t> indices16;
        indices16.reserve(data.m_indices.size());
        for (uint32_t idx : data.m_indices)
        {
            indices16.push_back(static_cast<uint16_t>(idx));
        }
        return indices16;
    }
}
