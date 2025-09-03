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
        m_shaderData = internalLoadShaderData(vs, fs);
    }

    void Shader::loadFromSource(const char* vs, const char* fs)
    {
        if (isValid == true)
        {
            unload();
        }
        isValid = true;
        m_shaderData = internalLoadShaderSrc(vs, fs);
    }

    void Shader::unload()
    {
        if (isValid == false)
        {
            return;
        }
        internalUnloadShaderData(m_shaderData);
        isValid = false;
    }

    int Shader::getUniformLocation(const string& name)
    {
        AIKO_NOT_IMPLEMENTED
    }

    void Shader::setBool(const string& name, bool value)
    {

    }

    void Shader::setInt(const string& name, int value)
    {

    }

    void Shader::setFloat(const string& name, float value)
    {

    }

    void Shader::setVec2(const string& name, const vec2& value)
    {

    }

    void Shader::setVec2(const string& name, float x, float y)
    {

    }

    void Shader::setVec3(const string& name, const vec3& value)
    {

    }
    void Shader::setVec3(const string& name, float x, float y, float z)
    {

    }

    void Shader::setVec4(const string& name, const vec4& value)
    {

    }

    void Shader::setVec4(const string& name, float x, float y, float z, float w)
    {

    }

    void Shader::setMat4(const string& name, const mat4& mat)
    {

    }

    void Shader::use()
    {

    }

    void Shader::unuse()
    {

    }

    aiko::ShaderData* Shader::getData()
    {
        return &m_shaderData;
    }

    void Shader::connect()
    {

    }

}
