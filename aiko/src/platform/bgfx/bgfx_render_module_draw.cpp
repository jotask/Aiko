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

    void BgfxRenderModule::renderMesh(Camera* cam,  Transform* transform, Mesh* mesh, Shader* shader)
    {

        const mat4 projMatrix = cam->getProjectionMatrix();
        const mat4 viewMatrix = cam->getViewMatrix();
        const mat4 modelMatrix = transform->getMatrix();

        ::bgfx::setViewTransform(m_kClearView, viewMatrix.data(), projMatrix.data());

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
            | BGFX_STATE_CULL_CW
            | BGFX_STATE_MSAA
        ;

        ::bgfx::setState(state);

        // Submit draw call
        ::bgfx::submit(m_kClearView, AIKO_TO_PH(shader->getData()->id));
    }

    void BgfxRenderModule::renderMesh(Camera* cam, Transform* transform, Mesh* mesh, Shader* shader, Texture* text)
    {

        const mat4 projMatrix = cam->getProjectionMatrix();
        const mat4 viewMatrix = cam->getViewMatrix();
        const mat4 modelMatrix = transform->getMatrix();

        ::bgfx::setViewTransform(m_kClearView, viewMatrix.data(), projMatrix.data());

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
        ::bgfx::submit(m_kClearView, AIKO_TO_PH(shader->getData()->id));

    }

    void BgfxRenderModule::renderMesh(Camera* cam, Transform* transform, Mesh* mesh, Shader* shader, texture::PboTexture* texture)
    {

        const mat4 projMatrix = cam->getProjectionMatrix();
        const mat4 viewMatrix = cam->getViewMatrix();
        const mat4 modelMatrix = transform->getMatrix();

        ::bgfx::setViewTransform(m_kClearView, viewMatrix.data(), projMatrix.data());

        const ::bgfx::UniformHandle sampler = AIKO_TO_UH(shader->getUniformLocation("u_texture"));
        ::bgfx::setTexture(0, sampler, AIKO_TO_TH(texture->texture.id));

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
        ::bgfx::submit(m_kClearView, AIKO_TO_PH(shader->getData()->id));

    }

}
#endif