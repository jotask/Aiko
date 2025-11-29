#include "render_factory.h"

#include "models/shader.h"

#if defined (AIKO_BGFX)
#include "platform/bgfx/bgfx_shader_impl.h"
#elif defined (AIKO_NATIVE)

#else
#error Backend not supported
#endif

namespace aiko
{
    namespace renderer
    {
        AikoPtr<interfaces::IShaderImpl> RendererFactory::createShaderImpl()
        {
            #if defined (AIKO_BGFX)
            return std::make_shared<bgfx::BgfxShaderImpl>();
            #elif defined (AIKO_NATIVE)
            return std::make_shared<OpenGLShaderImpl>();
            #else
            #error Backend not supported
            #endif
        }
    };
}