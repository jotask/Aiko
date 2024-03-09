#ifdef AIKO_BUILD_RAYLIB
#include "modules/render_module.h"

#include "core/libs.h"
#include "raylib_utils.h"

namespace aiko
{

    aiko::ShaderData RenderModule::loadShaderData(const char* vs, const char* fs)
    {
        ::Shader shader = LoadShader(vs, fs);
        return raylib::utils::toShader(shader);
    }

    void RenderModule::unloadShader(aiko::ShaderData& data)
    {
        ::Shader shader = raylib::utils::toRaylibShader(data);
        ::UnloadShader(shader);
    }

    int RenderModule::getShaderLocation(aiko::ShaderData& shader, const char* uniformName)
    {
        ::Shader m_shader = raylib::utils::toRaylibShader(shader);
        return ::GetShaderLocation(m_shader, uniformName);
    }

    void RenderModule::setShaderUniformValue(aiko::ShaderData& shader, int locIndex, const void* value, aiko::ShaderUniformDataType uniformType)
    {
        ::Shader m_shader = raylib::utils::toRaylibShader(shader);
        SetShaderValue(m_shader, locIndex, value, uniformType);
    }

}
#endif
