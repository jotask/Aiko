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
#include "platform/bgfx/bgfx_platform_helper.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <bgfx/bgfx.h>

#include "bgfx_platform_helper.h"

namespace aiko::bgfx
{

    #define AIKO_TO_VIEWID(view) static_cast<::bgfx::ViewId>(view)

    void BgfxRenderModule::renderMesh(Camera* cam,  Transform* transform, Mesh* mesh, Shader* shader)
    {

        const mat4 projMatrix = cam->getProjectionMatrix();
        const mat4 viewMatrix = cam->getViewMatrix();
        const mat4 modelMatrix = transform->getMatrix();

        ::bgfx::setViewTransform(AIKO_TO_VIEWID(currentViewId), viewMatrix.data(), projMatrix.data());

        // Set buffers
        ::bgfx::setVertexBuffer(0, AIKO_TO_VBH(mesh->m_data.vao));
        ::bgfx::setIndexBuffer(AIKO_TO_IBH(mesh->m_data.vbo));

        // Set transform
        ::bgfx::setTransform(modelMatrix.data());

        // Set default state
        constexpr const uint64_t state = 0
            | BGFX_STATE_WRITE_R
            | BGFX_STATE_WRITE_G
            | BGFX_STATE_WRITE_B
            | BGFX_STATE_WRITE_A
            | BGFX_STATE_WRITE_Z
            | BGFX_STATE_DEPTH_TEST_LESS
            // | BGFX_STATE_CULL_CW
            | BGFX_STATE_MSAA
        ;

        ::bgfx::setState(state);

        // Submit draw call
        ::bgfx::submit(AIKO_TO_VIEWID(currentViewId), AIKO_TO_PH(shader->getData()->id));
    }

    void BgfxRenderModule::renderMesh(Camera* cam, Transform* transform, Mesh* mesh, Shader* shader, Texture* text)
    {

        const mat4 projMatrix = cam->getProjectionMatrix();
        const mat4 viewMatrix = cam->getViewMatrix();
        const mat4 modelMatrix = transform->getMatrix();

        ::bgfx::setViewTransform(AIKO_TO_VIEWID(currentViewId), viewMatrix.data(), projMatrix.data());

        // Set buffers
        ::bgfx::setVertexBuffer(0, AIKO_TO_VBH(mesh->m_data.vao));
        ::bgfx::setIndexBuffer(AIKO_TO_IBH(mesh->m_data.vbo));

        // Set transform
        ::bgfx::setTransform(modelMatrix.data());

        // Set default state
        constexpr const uint64_t state = 0
            | BGFX_STATE_WRITE_R
            | BGFX_STATE_WRITE_G
            | BGFX_STATE_WRITE_B
            | BGFX_STATE_WRITE_A
            | BGFX_STATE_WRITE_Z
            | BGFX_STATE_DEPTH_TEST_LESS
            // | BGFX_STATE_CULL_CW
            | BGFX_STATE_MSAA
            ;

        ::bgfx::setState(state);

        const ::bgfx::UniformHandle sampler = AIKO_TO_UH(shader->getUniformLocation("u_texture"));

        ::bgfx::setTexture(0, sampler, AIKO_TO_TH(text->m_texture.id));

        // Submit draw call
        ::bgfx::submit(AIKO_TO_VIEWID(currentViewId), AIKO_TO_PH(shader->getData()->id));

    }

    void BgfxRenderModule::renderMesh(Camera* cam, Transform* transform, Mesh* mesh, Shader* shader, texture::PboTexture* texture)
    {

        const mat4 projMatrix = cam->getProjectionMatrix();
        const mat4 viewMatrix = cam->getViewMatrix();
        const mat4 modelMatrix = transform->getMatrix();

        ::bgfx::setViewTransform(AIKO_TO_VIEWID(currentViewId), viewMatrix.data(), projMatrix.data());

        const ::bgfx::UniformHandle sampler = AIKO_TO_UH(shader->getUniformLocation("u_texture"));
        ::bgfx::setTexture(0, sampler, AIKO_TO_TH(texture->texture.id), BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT);

        // Set buffers
        ::bgfx::setVertexBuffer(0, AIKO_TO_VBH(mesh->m_data.vao));
        ::bgfx::setIndexBuffer(AIKO_TO_IBH(mesh->m_data.vbo));

        // Set transform
        ::bgfx::setTransform(modelMatrix.data());

        // Set default state
        constexpr const uint64_t state = 0
            | BGFX_STATE_WRITE_R
            | BGFX_STATE_WRITE_G
            | BGFX_STATE_WRITE_B
            | BGFX_STATE_WRITE_A
            | BGFX_STATE_WRITE_Z
            | BGFX_STATE_DEPTH_TEST_LESS
            // | BGFX_STATE_CULL_CW
            | BGFX_STATE_MSAA
            ;

        ::bgfx::setState(state);

        // Submit draw call
        ::bgfx::submit(AIKO_TO_VIEWID(currentViewId), AIKO_TO_PH(shader->getData()->id));

    }

    void BgfxRenderModule::renderTransientBuffer(Camera* cam, Transform* transform, Shader* shader, std::vector<float>* vertices, std::vector<uint>* indices)
    {

        const mat4 projMatrix = cam->getProjectionMatrix();
        const mat4 viewMatrix = cam->getViewMatrix();
        const mat4 modelMatrix = transform->getMatrix();

        ::bgfx::setViewTransform(AIKO_TO_VIEWID(currentViewId), viewMatrix.data(), projMatrix.data());

        constexpr const uint strides = 3 + 2 + 3;

        const uint32_t numVertices = vertices->size() / strides;
        const uint32_t numIndices = indices->size();

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

        // Check if enough space is available for this frame
        if (::bgfx::getAvailTransientVertexBuffer(numVertices, ms_layout) >= numVertices &&
            ::bgfx::getAvailTransientIndexBuffer(numIndices) >= numIndices)
        {
            // Allocate per-frame buffers
            ::bgfx::TransientVertexBuffer tvb;
            ::bgfx::TransientIndexBuffer tib;

            ::bgfx::allocTransientVertexBuffer(&tvb, numVertices, ms_layout);
            ::bgfx::allocTransientIndexBuffer(&tib, numIndices);

            // Vertices
            PosTexColorVertex* verts = (PosTexColorVertex*)tvb.data;

            for (size_t i = 0; i < numVertices; ++i)
            {

                const size_t base = i * strides;

                PosTexColorVertex v{};
                // position
                v.x = (*vertices)[base + 0];
                v.y = (*vertices)[base + 1];
                v.z = (*vertices)[base + 2];

                // uv
                v.u = (*vertices)[base + 3];
                v.v = (*vertices)[base + 4];

                // color floats
                float rf = (*vertices)[base + 5];
                float gf = (*vertices)[base + 6];
                float bf = (*vertices)[base + 7];

                uint8_t r = static_cast<uint8_t>(rf * 255.0f);
                uint8_t g = static_cast<uint8_t>(gf * 255.0f);
                uint8_t b = static_cast<uint8_t>(bf * 255.0f);
                uint8_t a = 255;

                // pack to ABGR
                v.abgr = (uint32_t(a) << 24) | (uint32_t(b) << 16) | (uint32_t(g) << 8) | uint32_t(r);

                verts[i] = v;

            }

            // Fill indices

            uint16_t* ind = (uint16_t*)tib.data;
            for (size_t i = 0; i < numIndices; ++i)
            {
                ind[i] = (*indices)[i];
            }

            // Submit draw
            ::bgfx::setVertexBuffer(0, &tvb);
            ::bgfx::setIndexBuffer(&tib);

            ::bgfx::setTransform(modelMatrix.data());

            uint64_t state = 0
                | BGFX_STATE_WRITE_R
                | BGFX_STATE_WRITE_G
                | BGFX_STATE_WRITE_B
                | BGFX_STATE_WRITE_A
                | BGFX_STATE_WRITE_Z
                | BGFX_STATE_DEPTH_TEST_LESS
                // | BGFX_STATE_CULL_CW
                | BGFX_STATE_MSAA
                ;

            if (numVertices == 1)
            {
                state |= BGFX_STATE_PT_POINTS;
            }
            else if (numVertices == 2)
            {
                state |= BGFX_STATE_PT_LINES;
            }

            ::bgfx::setState(state);
            ::bgfx::submit(AIKO_TO_VIEWID(currentViewId), AIKO_TO_PH(shader->getData()->id));
        }
    }

}
#endif