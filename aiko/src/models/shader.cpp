#include "shader.h"

#include <stdexcept>

#include "core/libs.h"
#include "core/raylib_utils.h"

namespace aiko::shader
{
    
    Shader::Shader()
    {
    
    }

    void Shader::load(const char* vs, const char* fs)
    {
        auto data = LoadShader(vs, fs);
        m_id = data.id;
        m_locs = std::vector<int>( data.locs, data.locs + RL_MAX_SHADER_LOCATIONS);
    }

    void Shader::unload()
    {
        ::Shader m_shader = raylib::utils::toRaylibShader(*this);
        UnloadShader(m_shader);
    }

    int Shader::getShaderLocation(const char* locName)
    {
        ::Shader m_shader = raylib::utils::toRaylibShader(*this);
        return GetShaderLocation(m_shader, locName);
    }

    void Shader::setShaderValueV(int locIndex, const void* value, SUDT uniformType, int count)
    {
        ::Shader m_shader = raylib::utils::toRaylibShader(*this);
        SetShaderValueV(m_shader, locIndex, value, uniformType, count);
    }

    void Shader::setShaderValue(int locIndex, const void* value, SUDT uniformType)
    {
        ::Shader m_shader = raylib::utils::toRaylibShader(*this);
        SetShaderValue(m_shader, locIndex, &value, uniformType);
    }

    void Shader::setShaderValue(int locIndex, const int& value)
    {
        ::Shader m_shader = raylib::utils::toRaylibShader(*this);
        SetShaderValue(m_shader, locIndex, &value, SHADER_UNIFORM_INT);
    }

    void Shader::setShaderValue(int locIndex, const float& value)
    {
        ::Shader m_shader = raylib::utils::toRaylibShader(*this);
        SetShaderValue(m_shader, locIndex, &value, SHADER_UNIFORM_FLOAT);
    }

    void Shader::draw(texture::RenderTexture2D* text)
    {
        ::Shader m_shader = raylib::utils::toRaylibShader(*this);
        BeginShaderMode(m_shader);
        DrawTextureEx(raylib::utils::toRaylibTexture(text->texture), {0.0f, 0.0f}, 0.0f, 1.0f, ::WHITE);
        EndShaderMode();
    }

}
