#include "shader.h"

#include <stdexcept>
#include <algorithm>
#include <array>

#include "aiko.h"
#include "systems/render_system.h"
#include "systems/asset_system.h"

// FIXME
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace aiko
{

    #define AIKO_RETURN_NO_LOC if(locIndex < 0) return;

    Shader::Shader()
        : m_renderSystem(nullptr)
    {
    
    }

    void Shader::load(const char* vs, const char* fs)
    {
        // TODO unload if we have a shader loaded?
        // m_shaderData = m_assetSystem->getRenderSystem()->loadShaderData(vs, fs);
    }

    // void Shader::unload()
    // {
    //     // m_assetSystem->getRenderSystem()->unloadShader(*this);
    // }

    int Shader::getShaderLocation(const char* locName)
    {
        return m_renderSystem->getShaderLocation( *this ,locName);
    }

    // utility uniform functions
// ------------------------------------------------------------------------
    void Shader::setBool(const std::string& name, bool value) const
    {
        auto ID = m_shaderData.id;
        glUniform1i(glGetUniformLocation(m_shaderData.id, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void Shader::setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(m_shaderData.id, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void Shader::setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(m_shaderData.id, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void Shader::setVec2(const std::string& name, const vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(m_shaderData.id, name.c_str()), 1, &value.x);
    }
    void Shader::setVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(m_shaderData.id, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void Shader::setVec3(const std::string& name, const vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(m_shaderData.id, name.c_str()), 1, &value.x);
    }
    void Shader::setVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(m_shaderData.id, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void Shader::setVec4(const std::string& name, const vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(m_shaderData.id, name.c_str()), 1, &value.x);
    }

    void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(m_shaderData.id, name.c_str()), x, y, z, w);
    }
    void Shader::setMat4(const std::string& name, const mat4& mat) const
    {
        auto loc = glGetUniformLocation(m_shaderData.id, name.c_str());
        glUniformMatrix4fv( loc, 1, GL_FALSE, mat.data());
    }

    void Shader::setShaderValue(int locIndex, const int& value)
    {
        AIKO_RETURN_NO_LOC
        m_renderSystem->setShaderUniformValue(*this, locIndex, &value, SHADER_UNIFORM_INT);
    }

    void Shader::setShaderValue(int locIndex, const float& value)
    {
        AIKO_RETURN_NO_LOC
            m_renderSystem->setShaderUniformValue(*this, locIndex, &value, SHADER_UNIFORM_FLOAT);
    }

    void Shader::setShaderValue(int locIndex, const ivec2& value)
    {
        AIKO_RETURN_NO_LOC
            std::array<float, 2> loc;
        loc = { static_cast<float>(value.x), static_cast<float>(value.y) };
        m_renderSystem->setShaderUniformValue(*this, locIndex, loc.data(), SHADER_UNIFORM_VEC2);
    }

    void Shader::setShaderValue(int locIndex, const vec2& value)
    {
        AIKO_RETURN_NO_LOC
            std::array<float, 2> loc;
        loc = { static_cast<float>(value.x), static_cast<float>(value.y) };
        m_renderSystem->setShaderUniformValue(*this, locIndex, loc.data(), SHADER_UNIFORM_VEC2);
    }

    void Shader::setShaderValue(int locIndex, const vec3& value)
    {
        AIKO_RETURN_NO_LOC
        static std::array<float, 3> loc;
        loc = { value.x, value.y ,value.z };
        m_renderSystem->setShaderUniformValue(*this, locIndex, loc.data(), SHADER_UNIFORM_VEC3);
    }

    void Shader::setShaderValue(int locIndex, const vec4& value)
    {
        AIKO_RETURN_NO_LOC
            static std::array<float, 4> loc;
        loc = { value.x, value.y ,value.z, value.w };
        m_renderSystem->setShaderUniformValue(*this, locIndex, loc.data(), SHADER_UNIFORM_VEC4);
    }

    void Shader::setShaderValue(int locIndex, const std::vector<vec2>& value)
    {
        AIKO_RETURN_NO_LOC;
        m_renderSystem->setShaderUniformValueV(*this, locIndex, value.data(), SHADER_UNIFORM_VEC2, value.size());
    }

    aiko::ShaderData* Shader::getData()
    {
        return &m_shaderData;
    }

    void Shader::connect()
    {
        // m_renderSystem = m_assetSystem->getRenderSystem();
    }

}
