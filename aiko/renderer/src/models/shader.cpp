#include "shader.h"

#include <algorithm>
#include <utility>

#include <core/file.h>

#include "render_factory.h"

namespace aiko
{

    Shader::Shader()
        : backend(renderer::RendererFactory::createShaderImpl())
    {
        AIKO_ASSERT(backend != nullptr, "RendererFactory::createShaderImpl returned null");
    }

    RenderResourceId Shader::id() const
    {
        return backend->id();
    }

    void Shader::load(const char* fileCodeName)
    {
        std::string vs = std::string(fileCodeName) + ".vs";
        std::string fs = std::string(fileCodeName) + ".fs";
        this->load(vs.c_str(), fs.c_str());
        AIKO_ASSERT(isValid(), "Shader is invalid");
    }

    void Shader::load(const char* vs, const char* fs)
    {
        if (isValid() == true)
        {
            unload();
        }
        backend->load(vs, fs);
        AIKO_ASSERT(isValid(), "Shader is invalid");
    }

    void Shader::unload()
    {
        if (backend->isValid() == true)
        {
            backend->unload();
        }
        m_uniforms.clear();
    }

    void Shader::setUniform(const string& name, UniformValue value)
    {
        AIKO_ASSERT(name.empty() == false, "Uniform name cannot be empty");
        m_uniforms.insert_or_assign(name, std::move(value));
    }

    void Shader::setBool(const string& name, bool value)
    {
        setUniform(name, value);
    }

    void Shader::setInt(const string& name, int value)
    {
        setUniform(name, value);
    }

    void Shader::setFloat(const string& name, float value)
    {
        setUniform(name, value);
    }

    void Shader::setVec2(const string& name, const vec2& value)
    {
        setUniform(name, value);
    }

    void Shader::setVec2(const string& name, float x, float y)
    {
        setUniform(name, vec2{x, y});
    }

    void Shader::setVec3(const string& name, const vec3& value)
    {
        setUniform(name, value);
    }

    void Shader::setVec3(const string& name, float x, float y, float z)
    {
        setUniform(name, vec3{x, y, z});
    }

    void Shader::setVec4(const string& name, const vec4& value)
    {
        setUniform(name, value);
    }

    void Shader::setVec4(const string& name, float x, float y, float z, float w)
    {
        setUniform(name, vec4{x, y, z, w});
    }

    void Shader::setMat4(const string& name, const mat4& mat)
    {
        setUniform(name, mat);
    }

    void Shader::setUInt(const string& name, u32 value)
    {
        setUniform(name, value);
    }

    bool Shader::isValid() const
    {
        return backend->isValid();
    }

}
