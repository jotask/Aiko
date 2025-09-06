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
            .add(::bgfx::Attrib::Position, 3,   ::bgfx::AttribType::Float)
            .add(::bgfx::Attrib::TexCoord0, 2,  ::bgfx::AttribType::Float)
            .add(::bgfx::Attrib::Color0, 3,     ::bgfx::AttribType::Float, true) // normalized
            .end();

        ::bgfx::VertexBufferHandle vbh = ::bgfx::createVertexBuffer(
            ::bgfx::makeRef(mesh->m_vertices.data(), sizeof(mesh->m_vertices)),
            ms_layout
        );

        ::bgfx::IndexBufferHandle ibh = ::bgfx::createIndexBuffer(
            ::bgfx::makeRef(mesh->m_indices.data(), sizeof(mesh->m_indices.data()))
        );

        mesh->m_data.vao = vbh.idx;
        mesh->m_data.vbo = ibh.idx;

    }

    void BgfxRenderModule::renderMesh(Camera* cam,  Transform* transform, Mesh* mesh, Shader* shader)
    {
        ::bgfx::setIndexBuffer(AIKO_TO_IBH(mesh->m_data.vbo));
        ::bgfx::setVertexBuffer(0, AIKO_TO_VBH(mesh->m_data.vao));
        ::bgfx::submit(
            m_kClearView
            , AIKO_TO_PH(shader->getData()->id)
            , 0
            , BGFX_DISCARD_INDEX_BUFFER
            | BGFX_DISCARD_VERTEX_STREAMS
        );
        // AIKO_DEBUG_BREAK
    }

    void BgfxRenderModule::renderMesh(Camera* cam, Transform* transform, Mesh* mesh, Shader* shader, texture::Texture* text)
    {
        AIKO_DEBUG_BREAK
    }

}
#endif