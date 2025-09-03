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
#include "models/mesh.h"
#include "models/shader.h"
#include "components/transform_component.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace aiko::bgfx
{

    void BgfxRenderModule::refreshShader(Mesh*)
    {

    }

    AikoPtr<Shader> BgfxRenderModule::createShader()
    {
        AIKO_NOT_IMPLEMENTED
    }

    aiko::ShaderData BgfxRenderModule::loadShaderData(const char* vertex, const char* fragment)
    {
        AIKO_NOT_IMPLEMENTED
    }

    aiko::ShaderData BgfxRenderModule::loadShaderSrc(const char* vertex, const char* fragment)
    {
        AIKO_NOT_IMPLEMENTED
    }

    void BgfxRenderModule::unloadShader(aiko::ShaderData& data)
    {
        AIKO_NOT_IMPLEMENTED
    }

}
#endif