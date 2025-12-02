#include "render_factory.h"

#if defined (AIKO_BGFX)
#include "platform/bgfx/bgfx_shader_impl.h"
#include "platform/bgfx/bgfx_texture_impl.h"
#include "platform/bgfx/bgfx_mesh_impl.h"
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