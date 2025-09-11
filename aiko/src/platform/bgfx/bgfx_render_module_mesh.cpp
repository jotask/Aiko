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
#include "bgfx_render_utils.h"

namespace aiko::bgfx
{

    void BgfxRenderModule::initMesh( Mesh* mesh )
    {
        // DELETE ME ???
        // AIKO_DEBUG_BREAK
    }

    void BgfxRenderModule::refreshMesh(Mesh* mesh)
    {

        auto convertVertexBufferFromMesh = [&](Mesh* mesh) -> ::bgfx::VertexBufferHandle
            {
                auto vertices = shared::convertToBgfxVertex(*mesh);
                const ::bgfx::Memory* mem = ::bgfx::copy(vertices.data(), static_cast<uint32_t>(vertices.size() * sizeof(shared::VertexInformation)));
                return ::bgfx::createVertexBuffer(mem, shared::s_global_layout);

            };

        auto convertIndexBufferFromMesh = [](Mesh* mesh) -> ::bgfx::IndexBufferHandle
            {
                auto indices = shared::convertToBgfxIndices(*mesh);
                const ::bgfx::Memory* mem = ::bgfx::copy(indices.data(), static_cast<uint32_t>(indices.size() * sizeof(uint16_t)));
                return ::bgfx::createIndexBuffer(mem);

            };

        ::bgfx::VertexBufferHandle vbh = convertVertexBufferFromMesh(mesh);
        ::bgfx::IndexBufferHandle ibh = convertIndexBufferFromMesh(mesh);

        mesh->m_data.vao = vbh.idx;
        mesh->m_data.vbo = ibh.idx;

    }

}
#endif