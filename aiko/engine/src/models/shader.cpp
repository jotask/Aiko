#include "shader.h"

#include <stdexcept>
#include <algorithm>
#include <array>

#include "aiko.h"
#include "systems/render_system.h"
#include "systems/asset_system.h"
#include "modules/render/render_module.h"
#include "core/libs.h"
#include "core/log.h"

namespace aiko
{

    #define AIKO_RETURN_NO_LOC if(locIndex < 0) return;

    RenderModule* Shader::s_renderModule = nullptr;

    Shader::Shader()
    {
    
    }

    void Shader::load(const char* fileCodeName)
    {
        std::string vs = std::string(fileCodeName) + ".vs";
        std::string fs = std::string(fileCodeName) + ".fs";
        this->load(vs.c_str(), fs.c_str());
        assert(isvalid() && "Shader is invalid");
    }

    void Shader::load(const char* vs, const char* fs)
    {
        if (isValid == true)
        {
            unload();
        }
        isValid = true;
        m_shaderData = s_renderModule->loadShaderData(vs, fs);
    }

    void Shader::loadFromSource(const char* vs, const char* fs)
    {
        if (isValid == true)
        {
            unload();
        }
        isValid = true;
        m_shaderData = s_renderModule->loadShaderData(vs, fs);
    }

    void Shader::unload()
    {
        if (isValid == false)
        {
            return;
        }
        s_renderModule->unloadShader(m_shaderData);
        isValid = false;
    }

    int Shader::getUniformLocation(const string& name)
    {
        auto value = m_shaderData.locs.find(name);
        if (value != m_shaderData.locs.end())
        {
            return value->second;
        }
        Log::error( "UniformNotLoaded %s", name.c_str());
        assert(false);
        return { 0 };
    }

    void Shader::setBool(const string& name, bool value)
    {
        s_renderModule->setShaderUniform(this, name, value);
    }

    void Shader::setInt(const string& name, int value)
    {
        s_renderModule->setShaderUniform(this, name, value);
    }

    void Shader::setFloat(const string& name, float value)
    {
        s_renderModule->setShaderUniform(this, name, value);
    }

    void Shader::setVec2(const string& name, const vec2& value)
    {
        AIKO_DEBUG_BREAK
    }

    void Shader::setVec2(const string& name, float x, float y)
    {
        AIKO_DEBUG_BREAK
    }

    void Shader::setVec3(const string& name, const vec3& value)
    {
        AIKO_DEBUG_BREAK
    }
    void Shader::setVec3(const string& name, float x, float y, float z)
    {
        AIKO_DEBUG_BREAK
    }

    void Shader::setVec4(const string& name, const vec4& value)
    {
        s_renderModule->setShaderUniform(this, name, value);
    }

    void Shader::setVec4(const string& name, float x, float y, float z, float w)
    {
        AIKO_DEBUG_BREAK
    }

    void Shader::setMat4(const string& name, const mat4& mat)
    {
        AIKO_DEBUG_BREAK
    }

    void Shader::use()
    {
        s_renderModule->beginShaderMode(this);
    }

    void Shader::unuse()
    {
        s_renderModule->endShaderMode();
    }

    aiko::ShaderData* Shader::getData()
    {
        return &m_shaderData;
    }

    void Shader::connect()
    {
        AIKO_DEBUG_BREAK
    }

}
