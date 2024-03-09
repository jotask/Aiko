#include "shader.h"

#include <stdexcept>
#include <algorithm>
#include <array>

#include "aiko.h"
#include "systems/render_system.h"
#include "systems/asset_system.h"

namespace aiko::asset
{

#define AIKO_RETURN_NO_LOC if(locIndex < 0) return;

    Shader::Shader()
        : m_renderSystem(nullptr)
    {
    
    }

    void Shader::load(const char* vs, const char* fs)
    {
        // TODO unload if we have a shader loaded?
        m_shaderData = m_assetSystem->getRenderSystem()->loadShaderData(vs, fs);
    }

    void Shader::unload()
    {
        m_assetSystem->getRenderSystem()->unloadShader(*this);
    }

    int Shader::getShaderLocation(const char* locName)
    {
        return m_renderSystem->getShaderLocation( *this ,locName);
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

    void Shader::connect()
    {
        m_renderSystem = m_assetSystem->getRenderSystem();
    }

}
