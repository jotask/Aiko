#include "shader.h"

#include <stdexcept>
#include <algorithm>

#include "core/libs.h"
#include "core/raylib_utils.h"

namespace aiko::shader
{

#define AIKO_RETURN_NO_LOC if(locIndex< 0) return;

    Shader::Shader()
    {
    
    }

    void Shader::load(const char* vs, const char* fs)
    {
        auto data = LoadShader(vs, fs);
        m_id = data.id;
        m_locs.clear();
        m_locs.reserve( RL_MAX_SHADER_LOCATIONS);
        const int* end = data.locs + sizeof(data.locs);
        for (int* it = data.locs; it < end ; ++it)
        {
            int value = *it ;
            m_locs.push_back(value);
        }
    }

    void Shader::unload()
    {
        ::Shader shader = raylib::utils::toRaylibShader(*this);
        UnloadShader(shader);
    }

    int Shader::getShaderLocation(const char* locName)
    {
        ::Shader m_shader = raylib::utils::toRaylibShader(*this);
        return GetShaderLocation(m_shader, locName);
    }

    void Shader::setShaderValueV(int locIndex, const void* value, SUDT uniformType, int count)
    {
        AIKO_RETURN_NO_LOC
        ::Shader m_shader = raylib::utils::toRaylibShader(*this);
        SetShaderValueV(m_shader, locIndex, value, uniformType, count);
    }

    void Shader::setShaderValue(int locIndex, const void* value, SUDT uniformType)
    {
        AIKO_RETURN_NO_LOC
        ::Shader m_shader = raylib::utils::toRaylibShader(*this);
        SetShaderValue(m_shader, locIndex, &value, uniformType);
    }

    void Shader::setShaderValue(int locIndex, const int& value)
    {
        AIKO_RETURN_NO_LOC
        ::Shader m_shader = raylib::utils::toRaylibShader(*this);
        SetShaderValue(m_shader, locIndex, &value, SHADER_UNIFORM_INT);
    }

    void Shader::setShaderValue(int locIndex, const float& value)
    {
        AIKO_RETURN_NO_LOC
        ::Shader m_shader = raylib::utils::toRaylibShader(*this);
        SetShaderValue(m_shader, locIndex, &value, SHADER_UNIFORM_FLOAT);
    }

}
