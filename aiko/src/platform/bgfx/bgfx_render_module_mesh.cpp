#ifdef AIKO_BGFX

#include "bgfx_render_module.h"

#include <fstream>

#include <core/libs.h>

#include "modules/module_connector.h"
#include "modules/display_module.h"
#include "models/camera.h"
#include "types/textures.h"
#include "events/events.hpp"
#include "types/render_types.h"
#include "models/camera.h"
#include "models/mesh.h"
#include "models/shader.h"
#include "components/transform_component.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <bgfx/bgfx.h>

#include "bgfx_platform_helper.h"

namespace aiko::bgfx
{

    void BgfxRenderModule::initMesh( Mesh* mesh )
    {
        // DELETE ME ???
        // AIKO_DEBUG_BREAK
    }

    void BgfxRenderModule::refreshMesh(Mesh* mesh)
    {

        ::bgfx::VertexLayout ms_layout;
        ms_layout
            .begin()
            .add(::bgfx::Attrib::Position, 3, ::bgfx::AttribType::Float)
            .add(::bgfx::Attrib::TexCoord0, 2, ::bgfx::AttribType::Float)
            .add(::bgfx::Attrib::Color0, 4, ::bgfx::AttribType::Uint8, true)
            .end();

        struct PosTexColorVertex
        {
            float x, y, z;   // position
            float u, v;      // texcoord
            uint32_t abgr;   // color
        };

        auto convertVertexBufferFromMesh = [&](Mesh* mesh) -> ::bgfx::VertexBufferHandle
            {
                std::vector<PosTexColorVertex> vertices;

                const size_t stride = 8; // 3 pos + 2 uv + 3 color floats
                for (size_t i = 0; i < mesh->m_vertices.size(); i += stride)
                {
                    PosTexColorVertex v{};

                    // position
                    v.x = mesh->m_vertices[i + 0];
                    v.y = mesh->m_vertices[i + 1];
                    v.z = mesh->m_vertices[i + 2];

                    // uv
                    v.u = mesh->m_vertices[i + 3];
                    v.v = mesh->m_vertices[i + 4];

                    // color floats
                    float rf = mesh->m_vertices[i + 5];
                    float gf = mesh->m_vertices[i + 6];
                    float bf = mesh->m_vertices[i + 7];

                    uint8_t r = static_cast<uint8_t>(rf * 255.0f);
                    uint8_t g = static_cast<uint8_t>(gf * 255.0f);
                    uint8_t b = static_cast<uint8_t>(bf * 255.0f);
                    uint8_t a = 255;

                    // pack to ABGR
                    v.abgr = (uint32_t(a) << 24) | (uint32_t(b) << 16) | (uint32_t(g) << 8) | uint32_t(r);

                    vertices.push_back(v);
                }

                // bgfx::copy so BGFX owns the memory
                const ::bgfx::Memory* mem = ::bgfx::copy(vertices.data(), static_cast<uint32_t>(vertices.size() * sizeof(PosTexColorVertex)));
                return ::bgfx::createVertexBuffer(mem, ms_layout);

            };

        auto convertIndexBufferFromMesh = [](Mesh* mesh) -> ::bgfx::IndexBufferHandle
            {
                if(!mesh || mesh->m_indices.empty())
                {
                    return ::bgfx::IndexBufferHandle{ { ::bgfx::kInvalidHandle } };
                }

                std::vector<uint16_t> indices16;
                indices16.reserve(mesh->m_indices.size());
                for (uint32_t idx : mesh->m_indices)
                {
                    indices16.push_back(static_cast<uint16_t>(idx));
                }
                const ::bgfx::Memory* mem = ::bgfx::copy(indices16.data(), static_cast<uint32_t>(indices16.size() * sizeof(uint16_t)));
                return ::bgfx::createIndexBuffer(mem);

            };

        ::bgfx::VertexBufferHandle vbh = convertVertexBufferFromMesh(mesh);
        ::bgfx::IndexBufferHandle ibh = convertIndexBufferFromMesh(mesh);

        mesh->m_data.vao = vbh.idx;
        mesh->m_data.vbo = ibh.idx;

    }

}
#endif