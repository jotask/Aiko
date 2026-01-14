#include "render_module.h"

#include "modules/module_connector.h"

#include <aiko_renderer.h>

namespace aiko
{
    RenderModule::RenderModule(Aiko* aiko)
        : BaseModule(aiko)
    {
    }

    void RenderModule::init()
    {
        AikoRenderer::it().init();
    }

    void RenderModule::beginFrame()
    {
        AikoRenderer::it().beginFrame();
    }

    void RenderModule::endFrame()
    {
        AikoRenderer::it().endFrame();
    }

    void RenderModule::dispose()
    {
        AikoRenderer::it().dispose();
    }

}
