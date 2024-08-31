#include "platform_builder.hpp"

#define PLATFORM_NOT_SUPPORTED static_assert(false, "PLATFORM NOT SUPPORTED");

#if defined(AIKO_NATIVE)
    #include "platform/opengl/opengl_render_module.h"
    #include "platform/opengl/opengl_display_module.h"
#elif defined(AIKO_BGFX)
    #include "platform/bgfx/bgfx_render_module.h"
    #include "platform/bgfx/bgfx_display_module.h"
#else
    PLATFORM_NOT_SUPPORTED
#endif

namespace aiko::modules::builder
{

    AikoUPtr<aiko::RenderModule> getRenderSystem(Aiko* aiko)
    {
#if defined(AIKO_NATIVE)
        return std::make_unique<native::OpenglRenderModule>(aiko);
#elif defined(AIKO_BGFX)
        return std::make_unique<fx::BgfxRenderModule>(aiko);
#else
        PLATFORM_NOT_SUPPORTED
#endif
    }

    AikoUPtr<aiko::DisplayModule> getDisplaySystem(Aiko* aiko)
    {
#if defined(AIKO_NATIVE)
        return std::make_unique<native::OpenglDisplayModule>(aiko);
#elif defined(AIKO_BGFX)
        return std::make_unique<fx::BgfxDisplayModule>(aiko);
#else
        PLATFORM_NOT_SUPPORTED
#endif
    }

}
