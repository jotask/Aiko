#include "render_factory.h"

#if defined (AIKO_BGFX)
#include "platform/bgfx/bgfx_render_device.h"
#include "platform/bgfx/impl/bgfx_shader_impl.h"
#include "platform/bgfx/impl/bgfx_texture_impl.h"
#include "platform/bgfx/impl/bgfx_mesh_impl.h"
#include "platform/bgfx/impl/bgfx_framebuffer_impl.h"
#include "platform/bgfx/impl/bgfx_screenfbo_impl.h"
#elif defined (AIKO_NATIVE)

#else
#error Backend not supported
#endif

namespace aiko
{
    namespace renderer
    {
        AikoPtr<IRenderDevice> RendererFactory::createRenderDevice()
        {
            #if defined (AIKO_BGFX)
            return std::make_shared<bgfx::BgfxRenderDevice>();
            #elif defined (AIKO_NATIVE)
            #error Not implemented
            return std::make_shared<OpenGLShaderImpl>();
            #else
            #error Backend not supported
            #endif
        }

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

        AikoPtr<interfaces::ITextureImpl> RendererFactory::createTextureImpl()
        {
            #if defined (AIKO_BGFX)
            return std::make_shared<bgfx::BgfxTextureImpl>();
            #elif defined (AIKO_NATIVE)
            return std::make_shared<OpenGLShaderImpl>();
            #else
            #error Backend not supported
            #endif
        }

        AikoPtr<interfaces::IFrameBufferImpl> RendererFactory::createFrameBufferImpl()
        {
            #if defined (AIKO_BGFX)
            return std::make_shared<bgfx::BgfxFrameBufferImpl>();
            #elif defined (AIKO_NATIVE)
            return std::make_shared<OpenGLShaderImpl>();
            #else
            #error Backend not supported
            #endif
        }

        AikoPtr<interfaces::IScreenFboImpl> RendererFactory::createScreenFboImpl()
        {
            #if defined (AIKO_BGFX)
            return std::make_shared<bgfx::BgfxScreenFboImpl>();
            #elif defined (AIKO_NATIVE)
            return std::make_shared<OpenGLShaderImpl>();
            #else
            #error Backend not supported
            #endif
        }

        AikoPtr<interfaces::IMeshImpl> RendererFactory::createMeshImpl(Mesh* mesh)
        {
            #if defined (AIKO_BGFX)
            return std::make_shared<bgfx::BgfxMeshImpl>(mesh);
            #elif defined (AIKO_NATIVE)
            return std::make_shared<OpenGLShaderImpl>();
            #else
            #error Backend not supported
            #endif
        }
    };
}