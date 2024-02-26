#include "shader.h"

#include <stdexcept>

namespace aiko
{
    
    ShaderPtr::ShaderPtr()
    {
    
    }

    void ShaderPtr::load(const char* vs, const char* fs)
    {
        m_shader = LoadShader(vs, fs);
    }

    void ShaderPtr::unload()
    {
        UnloadShader(m_shader);
    }

    int ShaderPtr::getShaderLocation(const char* locName)
    {
        return GetShaderLocation(m_shader, locName);
    }

    void ShaderPtr::setShaderValueV(int locIndex, const void* value, SUDT uniformType, int count)
    {
        SetShaderValueV(m_shader, locIndex, value, uniformType, count);
    }

    void ShaderPtr::setShaderValue(int locIndex, const void* value, SUDT uniformType)
    {
        SetShaderValue(m_shader, locIndex, &value, uniformType);
    }

    void ShaderPtr::setShaderValue(int locIndex, const int& value)
    {
        SetShaderValue(m_shader, locIndex, &value, SHADER_UNIFORM_INT);
    }

    void ShaderPtr::setShaderValue(int locIndex, const float& value)
    {
        SetShaderValue(m_shader, locIndex, &value, SHADER_UNIFORM_FLOAT);
    }

    void ShaderPtr::draw(RenderTexture2D* text)
    {
        BeginShaderMode(m_shader);
        DrawTextureEx( text->texture, { 0.0f, 0.0f }, 0.0f, 1.0f, WHITE);
        EndShaderMode();
    }

}
