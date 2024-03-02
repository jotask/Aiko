#include "shader.h"

#include <stdexcept>
#include <algorithm>

#include "core/libs.h"
#include "core/raylib_utils.h"

namespace aiko::shader
{

#define AIKO_RETURN_NO_LOC if(locIndex < 0) return;

    Shader::Shader()
        : m_locs( RL_MAX_SHADER_LOCATIONS, -1 )
    {
    
    }

    void Shader::load(const char* vs, const char* fs)
    {
        auto data = LoadShader(vs, fs);
        m_id = data.id;
        m_locs.clear();
        m_locs.resize(RL_MAX_SHADER_LOCATIONS);
        std::copy(data.locs, data.locs + RL_MAX_SHADER_LOCATIONS, m_locs.begin());
    }

    void Shader::unload()
    {
        ::Shader shader = raylib::utils::toRaylibShader(*this, true);
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
        SetShaderValue(m_shader, locIndex, value, uniformType);
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

    void Shader::setShaderValue(int locIndex, const ivec2& value)
    {
        AIKO_RETURN_NO_LOC
            ::Shader m_shader = raylib::utils::toRaylibShader(*this);
        std::array<float, 2> loc = { static_cast<float>(value.x), static_cast<float>(value.y) };
        SetShaderValue(m_shader, locIndex, loc.data(), SHADER_UNIFORM_VEC2);
    }

    void Shader::setShaderValue(int locIndex, const vec3& value)
    {
        AIKO_RETURN_NO_LOC
        ::Shader m_shader = raylib::utils::toRaylibShader(*this);
        std::array<float, 3> loc = { value.x, value.y ,value.z };
        SetShaderValue(m_shader, locIndex, loc.data(), SHADER_UNIFORM_VEC3);
    }

}
