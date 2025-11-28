#ifdef AIKO_BGFX

#include "bgfx_render_utils.h"

namespace aiko
{
    namespace bgfx::shared
    {

        ::bgfx::VertexLayout s_global_layout;

        void init()
        {
            s_global_layout
                .begin()
                .add(::bgfx::Attrib::Position, 3, ::bgfx::AttribType::Float)
                .add(::bgfx::Attrib::TexCoord0, 2, ::bgfx::AttribType::Float)
                .add(::bgfx::Attrib::Normal, 3, ::bgfx::AttribType::Float, true, true)
                .add(::bgfx::Attrib::Color0, 4, ::bgfx::AttribType::Uint8, true)
                .end();
        }

        std::vector<VertexInformation> convertToBgfxVertex(const Mesh& mesh)
        {

            std::vector<VertexInformation> vertices;

            for (size_t i = 0; i < mesh.m_vertices.size(); ++i)
            {
                VertexInformation v{};

                // position
                const auto vertex = mesh.m_vertices[i];
                v.x = vertex.x;
                v.y = vertex.y;
                v.z = vertex.z;

                // uv
                const auto textCoord = mesh.m_teexCoord[i];
                v.u = textCoord.x;
                v.v = textCoord.y;

                // color
                const auto color = mesh.m_colors[i];

                uint8_t r = static_cast<uint8_t>(color.r * 255.0f);
                uint8_t g = static_cast<uint8_t>(color.g * 255.0f);
                uint8_t b = static_cast<uint8_t>(color.b * 255.0f);
                uint8_t a = static_cast<uint8_t>(color.a * 255.0f);

                // pack to ABGR
                v.abgr = (uint32_t(a) << 24) | (uint32_t(b) << 16) | (uint32_t(g) << 8) | uint32_t(r);

                // Normal
                const auto normal = mesh.m_normals[i];
                v.n_x = normal.x;
                v.n_y = normal.y;
                v.n_z = normal.z;

                vertices.push_back(v);
            }

            return vertices;

            
        }

        std::vector<uint16_t> convertToBgfxIndices(const Mesh& mesh)
        {
            std::vector<uint16_t> indices16;
            indices16.reserve(mesh.m_indices.size());
            for (uint32_t idx : mesh.m_indices)
            {
                indices16.push_back(static_cast<uint16_t>(idx));
            }
            return indices16;
        }

    }
}

#endif