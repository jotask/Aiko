#ifdef AIKO_BGFX

#include "bgfx_render_module.h"

#include <fstream>
#include <filesystem>

#include "modules/module_connector.h"
#include "modules/display_module.h"
#include "models/camera.h"
#include "types/textures.h"
#include "events/events.hpp"
#include "types/render_types.h"
#include "models/mesh.h"
#include "models/shader.h"
#include "components/transform_component.h"
#include "constants.h"
#include "platform/bgfx/bgfx_platform_helper.h"

#include <bgfx/bgfx.h>


namespace aiko::bgfx
{

    void BgfxRenderModule::initScreenFbo()
    {
        // Postprocessing shader
        m_passthrought.load("posprocessing");
        assert(m_passthrought.isvalid() && "Processing shader invalid!");
    }

}
#endif