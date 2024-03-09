#include "shader.h"

#include <stdexcept>
#include <algorithm>
#include <array>

#include "modules/asset_module.h"
#include "modules/render_module.h"

namespace aiko::asset
{

#define AIKO_RETURN_NO_LOC if(locIndex < 0) return;

    Shader::Shader()
    {
    
    }

    void Shader::load(const char* vs, const char* fs)
    {
        // TODO unload if we have a shader loaded?
        m_shaderData = m_assetModule->getRenderModule()->loadShaderData(vs, fs);
    }

    void Shader::unload()
    {
        m_assetModule->getRenderModule()->unloadShader(m_shaderData);
    }

    int Shader::getShaderLocation(const char* locName)
    {
        auto* rs = m_assetModule->getRenderModule();
        return rs->getShaderLocation( this->m_shaderData ,locName);
    }

    void Shader::setShaderValueV(int locIndex, const void* value, ShaderUniformDataType uniformType, int count)
    {
        AIKO_RETURN_NO_LOC
        // TODO count?
        auto* rs = m_assetModule->getRenderModule();
        rs->setShaderUniformValue(this->m_shaderData, locIndex, value, uniformType);
    }

    void Shader::setShaderValue(int locIndex, const void* value, ShaderUniformDataType uniformType)
    {
        AIKO_RETURN_NO_LOC
        auto* rs = m_assetModule->getRenderModule();
        rs->setShaderUniformValue(this->m_shaderData, locIndex, value, uniformType);
    }

    void Shader::setShaderValue(int locIndex, const int& value)
    {
        AIKO_RETURN_NO_LOC
        auto* rs = m_assetModule->getRenderModule();
        rs->setShaderUniformValue(this->m_shaderData, locIndex, &value, SHADER_UNIFORM_INT);
    }

    void Shader::setShaderValue(int locIndex, const float& value)
    {
        AIKO_RETURN_NO_LOC
        auto* rs = m_assetModule->getRenderModule();
        rs->setShaderUniformValue(this->m_shaderData, locIndex, &value, SHADER_UNIFORM_FLOAT);
    }

    void Shader::setShaderValue(int locIndex, const ivec2& value)
    {
        AIKO_RETURN_NO_LOC
        auto* rs = m_assetModule->getRenderModule();
        std::array<float, 2> loc;
        loc = { static_cast<float>(value.x), static_cast<float>(value.y) };
        rs->setShaderUniformValue(this->m_shaderData, locIndex, loc.data(), SHADER_UNIFORM_VEC2);
    }

    void Shader::setShaderValue(int locIndex, const vec3& value)
    {
        AIKO_RETURN_NO_LOC
        auto* rs = m_assetModule->getRenderModule();
        static std::array<float, 3> loc;
        loc = { value.x, value.y ,value.z };
        rs->setShaderUniformValue(this->m_shaderData, locIndex, loc.data(), SHADER_UNIFORM_VEC3);
    }

}
