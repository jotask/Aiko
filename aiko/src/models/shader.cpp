#include "shader.h"

#include <stdexcept>
#include <algorithm>
#include <array>

#include "aiko.h"
#include "systems/render_system.h"
#include "systems/asset_system.h"
#include "modules/render_module.h"

// FIXME
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

namespace aiko
{

    #define AIKO_RETURN_NO_LOC if(locIndex < 0) return;

    Shader::Shader()
    {
    
    }

    void Shader::load(const char* vs, const char* fs)
    {
        if (isValid == true)
        {
            unload();
        }
        isValid = true;
        m_shaderData = loadShaderData(vs, fs);
        this->vs = vs;
        this->fs = fs;
    }

    void Shader::unload()
    {
        if (isValid == false)
        {
            return;
        }
        unloadShaderData(m_shaderData);
        isValid = false;
    }

    int Shader::getUniformLocation(const std::string& name)
    {
        use();
        auto found = m_shaderData.locs.find(name.c_str());
        if (found != m_shaderData.locs.end())
        {
            return found->second;
        }

        int loc = glGetUniformLocation(m_shaderData.id, name.c_str());
        if (loc == -1)
        {
            std::cout << "ERROR::SHADER::LOCALISATION NOT FOUND :: " << name.c_str() << std::endl;
        }
        return loc;
    }

    // utility uniform functions
// ------------------------------------------------------------------------
    void Shader::setBool(const std::string& name, bool value)
    {
        glUniform1i(getUniformLocation(name), (int)value);
    }
    // ------------------------------------------------------------------------
    void Shader::setInt(const std::string& name, int value)
    {
        glUniform1i(getUniformLocation(name), value);
    }
    // ------------------------------------------------------------------------
    void Shader::setFloat(const std::string& name, float value)
    {
        glUniform1f(getUniformLocation(name), value);
    }
    // ------------------------------------------------------------------------
    void Shader::setVec2(const std::string& name, const vec2& value)
    {
        glUniform2fv(getUniformLocation(name), 1, &value.x);
    }
    void Shader::setVec2(const std::string& name, float x, float y)
    {
        glUniform2f(getUniformLocation(name), x, y);
    }
    // ------------------------------------------------------------------------
    void Shader::setVec3(const std::string& name, const vec3& value)
    {
        glUniform3fv(getUniformLocation(name), 1, &value.x);
    }
    void Shader::setVec3(const std::string& name, float x, float y, float z)
    {
        glUniform3f(getUniformLocation(name), x, y, z);
    }
    // ------------------------------------------------------------------------
    void Shader::setVec4(const std::string& name, const vec4& value)
    {
        glUniform4fv(getUniformLocation(name), 1, &value.x);
    }

    void Shader::setVec4(const std::string& name, float x, float y, float z, float w)
    {
        glUniform4f(getUniformLocation(name), x, y, z, w);
    }

    void Shader::setMat4(const std::string& name, const mat4& mat)
    {
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, mat.data());
    }

    void Shader::use()
    {
        glUseProgram(this->getData()->id);
    }

    void Shader::unuse()
    {
        glUseProgram(0);
    }

    /*
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
    */

    aiko::ShaderData* Shader::getData()
    {
        return &m_shaderData;
    }

    void Shader::connect()
    {

    }

}
