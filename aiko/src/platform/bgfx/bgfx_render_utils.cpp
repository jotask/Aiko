#ifdef AIKO_BGFX

#include "bgfx_render_utils.h"

namespace aiko
{
    namespace bgfx
    {

        void init()
        {
            s_global_layout
                .begin()
                .add(::bgfx::Attrib::Position, 3, ::bgfx::AttribType::Float)
                .add(::bgfx::Attrib::TexCoord0, 2, ::bgfx::AttribType::Float)
                .add(::bgfx::Attrib::Color0, 4, ::bgfx::AttribType::Uint8, true)
                .end();
        }

        std::vector<VertexInformation> convertTo(const Mesh& mesh)
        {

            std::vector<VertexInformation> vertices;

            const size_t stride = 8; // 3 pos + 2 uv + 3 color floats
            for (size_t i = 0; i < mesh.m_vertices.size(); i += stride)
            {
                VertexInformation v{};

                // position
                v.x = mesh.m_vertices[i + 0];
                v.y = mesh.m_vertices[i + 1];
                v.z = mesh.m_vertices[i + 2];

                // uv
                v.u = mesh.m_vertices[i + 3];
                v.v = mesh.m_vertices[i + 4];

                // color floats
                float rf = mesh.m_vertices[i + 5];
                float gf = mesh.m_vertices[i + 6];
                float bf = mesh.m_vertices[i + 7];

                uint8_t r = static_cast<uint8_t>(rf * 255.0f);
                uint8_t g = static_cast<uint8_t>(gf * 255.0f);
                uint8_t b = static_cast<uint8_t>(bf * 255.0f);
                uint8_t a = 255;

                // pack to ABGR
                v.abgr = (uint32_t(a) << 24) | (uint32_t(b) << 16) | (uint32_t(g) << 8) | uint32_t(r);

                vertices.push_back(v);
            }

            return vertices;

            
        }

    }
}

#endif