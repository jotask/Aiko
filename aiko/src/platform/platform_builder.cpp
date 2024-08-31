#include "platform_builder.hpp"

#define PLATFORM_NOT_SUPPORTED static_assert(false, "PLATFORM NOT SUPPORTED");

#ifdef AIKO_NATIVE
    #include "platform/opengl/opengl_render_module.h"
    #include "platform/opengl/opengl_display_module.h"
#else
    PLATFORM_NOT_SUPPORTED
#endif

namespace aiko::modules::builder
{

    AikoUPtr<aiko::RenderModule> getRenderSystem(Aiko* aiko)
    {
#ifdef AIKO_NATIVE
        return std::make_unique<native::OpenglRenderModule>(aiko);
#else
        PLATFORM_NOT_SUPPORTED
#endif
    }

    AikoUPtr<aiko::DisplayModule> getDisplaySystem(Aiko* aiko)
    {
#ifdef AIKO_NATIVE
        return std::make_unique<native::OpenglDisplayModule>(aiko);
#else
        PLATFORM_NOT_SUPPORTED
#endif
    }

}
