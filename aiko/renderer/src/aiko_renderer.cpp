#include "aiko_renderer.h"

#if defined (AIKO_BGFX)
#include "platform/bgfx/bgfx_aiko_renderer.h"
#elif defined (AIKO_NATIVE)
#include "platform/bgfx/opengl_aiko_renderer.h"
#else
#error Backend not supported
#endif

namespace aiko
{
    AikoRenderer::AikoRenderer()
        #if defined (AIKO_BGFX)
            : m_renderer(std::make_shared<bgfx::BgfxRenderer>())
        #elif defined (AIKO_NATIVE)
            : m_renderer(std::make_shared<bgfx::BgfxShaderImpl>())
        #else
            #error Backend not supported
        #endif
    {

    }

    void AikoRenderer::init()
    {
        m_renderer->init();
    }

    void AikoRenderer::beginFrame()
    {
        m_renderer->beginFrame();
    }

    void AikoRenderer::endFrame()
    {
        m_renderer->endFrame();
    }

    void AikoRenderer::dispose()
    {
        m_renderer->dispose();
    }

    void AikoRenderer::setBackgroundColor(const Color color)
    {
        m_renderer->setBackgroundColor(color);
    }

    void AikoRenderer::render(Camera* cam, Transform* trans, Mesh* mesh, Shader* shader)
    {
        m_renderer->render(cam, trans, mesh, shader);
    }

    void AikoRenderer::render(Camera* cam, Transform* trans, Mesh* mesh, Shader* shader, Texture* texture)
    {
        m_renderer->render(cam, trans, mesh, shader, texture);
    }

    void AikoRenderer::render(Camera* cam, Transform* trans, Model* model)
    {
        m_renderer->render(cam, trans, model);
    }

    void AikoRenderer::render(Camera* cam, Transform* trans, Mesh* mesh, Shader* shader, FrameBuffer frame_buffer)
    {
        m_renderer->render(cam, trans, mesh, shader, frame_buffer);
    }

    void AikoRenderer::drawText(string str, float x, float y, float size, Color color)
    {
        m_renderer->drawText(str, x, y, size, color);
    }
}
