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
        /*
        ::bgfx::VertexLayout ms_layout;
        ms_layout
            .begin()
            .add(::bgfx::Attrib::Position, 3, ::bgfx::AttribType::Float)
            .add(::bgfx::Attrib::TexCoord0, 2, ::bgfx::AttribType::Float)
            .add(::bgfx::Attrib::Color0, 4, ::bgfx::AttribType::Uint8, true) // normalized
            .end();

        struct PosTexColorVertex
        {
            float x, y, z;   // position
            float u, v;      // texcoord
            uint32_t abgr;   // color
        };


        auto convertVertexBufferFromMesh = [](Mesh* mesh) -> ::bgfx::VertexBufferHandle
            {
                if (!mesh)
                {
                    return ::bgfx::VertexBufferHandle{ { ::bgfx::kInvalidHandle } };
                }

                std::vector<PosTexColorVertex> vertices;
                const size_t vertexCount = mesh->m_vertices.size() / 3; // 3 floats per vertex

                vertices.reserve(vertexCount);

                for (size_t i = 0; i < vertexCount; ++i)
                {
                    PosTexColorVertex vtx;

                    // Position
                    vtx.x = mesh->m_vertices[i * 3 + 0];
                    vtx.y = mesh->m_vertices[i * 3 + 1];
                    vtx.z = mesh->m_vertices[i * 3 + 2];

                    // Texcoord
                    if (!mesh->m_teexCoord.empty())
                    {
                        vtx.u = mesh->m_teexCoord[i * 2 + 0];
                        vtx.v = mesh->m_teexCoord[i * 2 + 1];
                    }
                    else
                    {
                        vtx.u = 0.0f;
                        vtx.v = 0.0f;
                    }

                    // Color
                    if (!mesh->m_colors.empty())
                    {
                        unsigned char r = mesh->m_colors[i * 4 + 0];
                        unsigned char g = mesh->m_colors[i * 4 + 1];
                        unsigned char b = mesh->m_colors[i * 4 + 2];
                        unsigned char a = mesh->m_colors[i * 4 + 3];
                        vtx.abgr = (r << 24) | (g << 16) | (b << 8) | a;
                    }
                    else
                    {
                        vtx.abgr = 0xffffffff; // white
                    }

                    vertices.push_back(vtx);
                }

                // Define vertex layout for vs_cubes
                ::bgfx::VertexLayout layout;
                layout.begin()
                    .add(::bgfx::Attrib::Position, 3, ::bgfx::AttribType::Float)
                    .add(::bgfx::Attrib::TexCoord0, 2, ::bgfx::AttribType::Float)
                    .add(::bgfx::Attrib::Color0, 4, ::bgfx::AttribType::Uint8, true) // normalized
                    .end();

                return ::bgfx::createVertexBuffer(
                    ::bgfx::makeRef(vertices.data(), uint32_t(vertices.size() * sizeof(PosTexColorVertex))),
                    layout
                );
            };

        auto convertIndexBufferFromMesh = [](Mesh* mesh) -> ::bgfx::IndexBufferHandle
            {
                if (!mesh || mesh->m_indices.empty())
                {
                    return ::bgfx::IndexBufferHandle{ { ::bgfx::kInvalidHandle } };
                }

                return ::bgfx::createIndexBuffer(
                    ::bgfx::makeRef(mesh->m_indices.data(), uint32_t(mesh->m_indices.size() * sizeof(uint32_t)))
                );
            };


        ::bgfx::VertexBufferHandle vbh = convertVertexBufferFromMesh(mesh);
        ::bgfx::IndexBufferHandle ibh = convertIndexBufferFromMesh(mesh);

        if (::bgfx::isValid(vbh) == false)
        {
            throw std::runtime_error("Failed to create VertexBufferHandle!");
        }

        if (::bgfx::isValid(ibh) == false)
        {
            throw std::runtime_error("Failed to create IndexBufferHandle!");
        }

        mesh->m_data.vao = vbh.idx;
        mesh->m_data.vbo = ibh.idx;

        */
    }

    void BgfxRenderModule::renderMesh(Camera* cam,  Transform* transform, Mesh* mesh, Shader* shader)
    {

        struct PosTexColorVertex
        {
            float x, y, z;
            float u, v;
            uint32_t abgr;
        };

        // simple cube vertices
        static PosTexColorVertex cubeVertices[] =
        {
            { -1.f, -1.f,  1.f, 0.f, 0.f, 0xffffffff },
            {  1.f, -1.f,  1.f, 1.f, 0.f, 0xffffffff },
            {  1.f,  1.f,  1.f, 1.f, 1.f, 0xffffffff },
            { -1.f,  1.f,  1.f, 0.f, 1.f, 0xffffffff },
            { -1.f, -1.f, -1.f, 0.f, 0.f, 0xffffffff },
            {  1.f, -1.f, -1.f, 1.f, 0.f, 0xffffffff },
            {  1.f,  1.f, -1.f, 1.f, 1.f, 0xffffffff },
            { -1.f,  1.f, -1.f, 0.f, 1.f, 0xffffffff },
        };

        // cube indices
        static uint16_t cubeIndices[] =
        {
            0,1,2, 0,2,3, // front
            1,5,6, 1,6,2, // right
            5,4,7, 5,7,6, // back
            4,0,3, 4,3,7, // left
            3,2,6, 3,6,7, // top
            4,5,1, 4,1,0  // bottom
        };

        // Compute model matrix
        mat4 modelMatrix = transform->getMatrix();

        // Compute camera view/projection matrices

        mat4 viewMatrix = cam->getViewMatrix();
        mat4 projMatrix = cam->getProjectionMatrix();

        // Set BGFX view and clear
        uint8_t viewId = 0; // default view
        ::bgfx::setViewClear(viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
        ::bgfx::setViewRect(viewId, 0, 0, ::bgfx::BackbufferRatio::Equal);

        ::bgfx::setViewTransform(viewId, viewMatrix.data(), projMatrix.data());

        // Create vertex layout
        static ::bgfx::VertexLayout s_layout;
        if (!s_layout.getStride())
        {
            s_layout.begin()
                .add(::bgfx::Attrib::Position, 3, ::bgfx::AttribType::Float)
                .add(::bgfx::Attrib::TexCoord0, 2, ::bgfx::AttribType::Float)
                .add(::bgfx::Attrib::Color0, 4, ::bgfx::AttribType::Uint8, true)
                .end();
        }

        // Create vertex buffer
        ::bgfx::VertexBufferHandle vbh = ::bgfx::createVertexBuffer(
            ::bgfx::makeRef(cubeVertices, sizeof(cubeVertices)),
            s_layout
        );

        // Create index buffer
        ::bgfx::IndexBufferHandle ibh = ::bgfx::createIndexBuffer(
            ::bgfx::makeRef(cubeIndices, sizeof(cubeIndices))
        );

        // Set buffers
        ::bgfx::setVertexBuffer(0, vbh);
        ::bgfx::setIndexBuffer(ibh);

        // Set transform
        ::bgfx::setTransform(modelMatrix.data());

        // Set default state
        uint64_t state = 0
            | BGFX_STATE_WRITE_R
            | BGFX_STATE_WRITE_G
            | BGFX_STATE_WRITE_B
            | BGFX_STATE_WRITE_A
            | BGFX_STATE_WRITE_Z
            | BGFX_STATE_DEPTH_TEST_LESS
            | BGFX_STATE_CULL_CW
            | BGFX_STATE_MSAA
        ;

        ::bgfx::setState(state);

        // Submit draw call
        ::bgfx::submit(viewId, AIKO_TO_PH(shader->getData()->id));
    }

    void BgfxRenderModule::renderMesh(Camera* cam, Transform* transform, Mesh* mesh, Shader* shader, texture::Texture* text)
    {
        AIKO_DEBUG_BREAK
    }

}
#endif