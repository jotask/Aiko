#include "bgfx_mesh_impl.h"

#include <core/file.h>

#include "models/mesh.h"
#include "platform/bgfx/bgfx_types.h"

namespace aiko::bgfx
{
    namespace shared {
        struct VertexInformation;
    }

    BgfxMeshImpl::BgfxMeshImpl(const Mesh* mesh)
        : IMeshImpl(mesh)
        , m_vertexBuffer({::bgfx::kInvalidHandle})
        , m_indexBuffer({::bgfx::kInvalidHandle})
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

    void BgfxMeshImpl::refresh()
    {

        if (::bgfx::isValid(m_vertexBuffer) == true)
        {
            ::bgfx::destroy(m_vertexBuffer);
        }

        const auto vertices = convertToVBH();
        const ::bgfx::Memory* memV = ::bgfx::copy(vertices.data(), static_cast<uint32_t>(vertices.size() * sizeof(VertexInformation)));
        m_vertexBuffer = ::bgfx::createVertexBuffer(memV, s_global_layout);;

        if (::bgfx::isValid(m_indexBuffer) == true)
        {
            ::bgfx::destroy(m_indexBuffer);
        }

        const auto indices = convertToIBH();
        const ::bgfx::Memory* memI = ::bgfx::copy(indices.data(), static_cast<uint32_t>(indices.size() * sizeof(uint16_t)));
        m_indexBuffer = ::bgfx::createIndexBuffer(memI);

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
            const auto textCoord = data.m_teexCoord[i];
            v.u = textCoord.x;
            v.v = textCoord.y;

            // color
            const auto color = data.m_colors[i];

            uint8_t r = static_cast<uint8_t>(color.r * 255.0f);
            uint8_t g = static_cast<uint8_t>(color.g * 255.0f);
            uint8_t b = static_cast<uint8_t>(color.b * 255.0f);
            uint8_t a = static_cast<uint8_t>(color.a * 255.0f);

            // pack to ABGR
            v.abgr = (uint32_t(a) << 24) | (uint32_t(b) << 16) | (uint32_t(g) << 8) | uint32_t(r);

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
