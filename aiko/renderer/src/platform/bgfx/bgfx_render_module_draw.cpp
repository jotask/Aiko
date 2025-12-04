#ifdef AIKO_BGFX

#include "bgfx_render_module.h"

#include <core/transform.h>

#include "display/display_manager.h"
#include "models/camera.h"
#include "models/mesh.h"
#include "models/shader.h"

#include "platform/bgfx/bgfx_types.h"
#include "platform/bgfx/impl/bgfx_shader_impl.h"
#include "platform/bgfx/impl/bgfx_mesh_impl.h"
#include "platform/bgfx/impl/bgfx_texture_impl.h"
#include "platform/bgfx/bgfx_platform_helper.h"

#include <bgfx/bgfx.h>

namespace aiko::bgfx
{

    #define AIKO_TO_VIEWID(view) static_cast<::bgfx::ViewId>(view)

    void BgfxRenderer::renderMesh(Camera* cam,  Transform* transform, Mesh* mesh, Shader* shader)
    {

        const mat4 projMatrix = cam->getProjectionMatrix();
        const mat4 viewMatrix = cam->getViewMatrix();
        const mat4 modelMatrix = transform->getMatrix();

        ::bgfx::setViewTransform(AIKO_TO_VIEWID(currentViewId), viewMatrix.data(), projMatrix.data());

        // Set buffers
        GET_BACKEND_IMPL(mesh->getImpl(), BgfxMeshImpl, m)
        ::bgfx::setVertexBuffer(0, m->getVertexBuffferHandler());
        ::bgfx::setIndexBuffer(m->getIndexBuffferHandler());

        // Set transform
        ::bgfx::setTransform(modelMatrix.data());

        ::bgfx::setState(s_default_state);

        // Submit draw call
        GET_BACKEND_IMPL(shader->getImpl(), BgfxShaderImpl, program);
        ::bgfx::submit(AIKO_TO_VIEWID(currentViewId), program->getProgramHandler());

    }

    void BgfxRenderer::renderMesh(Camera* cam, Transform* transform, Mesh* mesh, Shader* shader, Texture* text)
    {

        const mat4 projMatrix = cam->getProjectionMatrix();
        const mat4 viewMatrix = cam->getViewMatrix();
        const mat4 modelMatrix = transform->getMatrix();

        ::bgfx::setViewTransform(AIKO_TO_VIEWID(currentViewId), viewMatrix.data(), projMatrix.data());

        // Set buffers
        GET_BACKEND_IMPL(mesh->getImpl(), BgfxMeshImpl, m)
        ::bgfx::setVertexBuffer(0, m->getVertexBuffferHandler());
        ::bgfx::setIndexBuffer(m->getIndexBuffferHandler());

        // Set transform
        ::bgfx::setTransform(modelMatrix.data());

        ::bgfx::setState(s_default_state);

        GET_BACKEND_IMPL(shader->getImpl(), BgfxShaderImpl, program);
        const ::bgfx::UniformHandle sampler = program->getUniformHandle("u_texture");

        GET_BACKEND_IMPL(text->getImpl(), BgfxTextureImpl, texture)
        ::bgfx::setTexture(0, sampler, texture->getTextureHandler());

        // Submit draw call
        ::bgfx::submit(AIKO_TO_VIEWID(currentViewId), program->getProgramHandler());

    }

    void BgfxRenderer::renderMesh(Camera* cam, Transform* transform, Mesh* mesh, Shader* shader, AikoPtr<FrameBuffer> texture)
    {

        const mat4 projMatrix = cam->getProjectionMatrix();
        const mat4 viewMatrix = cam->getViewMatrix();
        const mat4 modelMatrix = transform->getMatrix();

        ::bgfx::setViewTransform(AIKO_TO_VIEWID(currentViewId), viewMatrix.data(), projMatrix.data());

        GET_BACKEND_IMPL(shader->getImpl(), BgfxShaderImpl, program);
        const ::bgfx::UniformHandle sampler = program->getUniformHandle("u_texture");

        GET_BACKEND_IMPL(texture->getImpl(), BgfxTextureImpl, text)
        ::bgfx::setTexture(0, sampler, text->getTextureHandler(), BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT);

        // Set buffers
        GET_BACKEND_IMPL(mesh->getImpl(), BgfxMeshImpl, m)
        ::bgfx::setVertexBuffer(0, m->getVertexBuffferHandler());
        ::bgfx::setIndexBuffer(m->getIndexBuffferHandler());

        // Set transform
        ::bgfx::setTransform(modelMatrix.data());

        ::bgfx::setState(s_default_state);

        // Submit draw call
        ::bgfx::submit(AIKO_TO_VIEWID(currentViewId), program->getProgramHandler());

    }


    void BgfxRenderer::renderModel(Camera* cam, Transform* transform, Model* model)
    {
        const mat4 projMatrix = cam->getProjectionMatrix();
        const mat4 viewMatrix = cam->getViewMatrix();
        const mat4 modelMatrix = transform->getMatrix();

        ::bgfx::setViewTransform(AIKO_TO_VIEWID(currentViewId), viewMatrix.data(), projMatrix.data());

        for (auto mod : model->m_meshes)
        {

            const auto* mesh = &mod.mesh;
            auto* material = &mod.material;

            // Set buffers
            GET_BACKEND_IMPL(mesh->getImpl(), BgfxMeshImpl, m)
            ::bgfx::setVertexBuffer(0, m->getVertexBuffferHandler());
            ::bgfx::setIndexBuffer(m->getIndexBuffferHandler());

            // Set transform
            ::bgfx::setTransform(modelMatrix.data());

            ::bgfx::setState(s_default_state);

            GET_BACKEND_IMPL(material->m_shader.getImpl(), BgfxShaderImpl, program);

            const auto co = material->m_baseColor;
            const ::bgfx::UniformHandle u_baseColor = program->getUniformHandle("u_baseColor");
            const float c[4] = { co.r, co.g, co.b, co.a };
            ::bgfx::setUniform(u_baseColor, &c);

            // x: use texture
            // y: use vertex color
            // z: basic lighting
            const ::bgfx::UniformHandle u_flags = program->getUniformHandle("u_flags");

		    const bool useTexture = material->m_diffuse.isValid();
		    const bool useVertexColor = false;
		    const bool useLighting = true;

            const float flags[4] = { TO_BGFX_BOOL(useTexture), TO_BGFX_BOOL(useVertexColor), TO_BGFX_BOOL(useLighting), 0.0f };
            ::bgfx::setUniform(u_flags, &flags);

            if (useTexture == true)
            {
                const ::bgfx::UniformHandle sampler = program->getUniformHandle("u_texture");

                GET_BACKEND_IMPL(material->m_diffuse.getImpl(), BgfxTextureImpl, texture)
                ::bgfx::setTexture(0, sampler, texture->getTextureHandler());
            }

            // Submit draw call
            ::bgfx::submit(AIKO_TO_VIEWID(currentViewId), program->getProgramHandler());

        }
    }

    void BgfxRenderer::renderTransientBuffer(Camera* cam, Transform* transform, Shader* shader, Mesh* mesh)
    {

        const mat4 projMatrix = cam->getProjectionMatrix();
        const mat4 viewMatrix = cam->getViewMatrix();
        const mat4 modelMatrix = transform->getMatrix();

        ::bgfx::setViewTransform(AIKO_TO_VIEWID(currentViewId), viewMatrix.data(), projMatrix.data());

        const uint32_t numVertices = mesh->m_vertices.size();
        const uint32_t numIndices  = mesh->m_indices.size();

        // Check if enough space is available for this frame
        if (::bgfx::getAvailTransientVertexBuffer(numVertices, s_global_layout) >= numVertices &&
            ::bgfx::getAvailTransientIndexBuffer(numIndices) >= numIndices)
        {
            // Allocate per-frame buffers
            ::bgfx::TransientVertexBuffer tvb;
            ::bgfx::TransientIndexBuffer tib;

            ::bgfx::allocTransientVertexBuffer(&tvb, numVertices, s_global_layout);
            ::bgfx::allocTransientIndexBuffer(&tib, numIndices);

            // Vertices
            VertexInformation* verts = (VertexInformation*)tvb.data;
            GET_BACKEND_IMPL(mesh->getImpl(), BgfxMeshImpl, m)

            const auto localVertex = m->convertToVBH();
            for (size_t i = 0; i < numVertices; ++i)
            {
                verts[i] = localVertex[i];
            }

            // Indices
            uint16_t* ind = (uint16_t*)tib.data;
            const auto localIndices = m->convertToIBH();
            for (size_t i = 0; i < numIndices; ++i)
            {
                ind[i] = localIndices[i];
            }

            // Submit draw
            ::bgfx::setVertexBuffer(0, &tvb);
            ::bgfx::setIndexBuffer(&tib);

            ::bgfx::setTransform(modelMatrix.data());

            uint64_t state = s_default_state;

            if (numVertices == 1)
            {
                state |= BGFX_STATE_PT_POINTS;
            }
            else if (numVertices == 2)
            {
                state |= BGFX_STATE_PT_LINES;
            }

            ::bgfx::setState(state);

            GET_BACKEND_IMPL(shader->getImpl(), BgfxShaderImpl, program);
            ::bgfx::submit(AIKO_TO_VIEWID(currentViewId), program->getProgramHandler());
        }
    }

}
#endif