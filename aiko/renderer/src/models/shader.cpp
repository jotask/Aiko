#include "shader.h"

#include <algorithm>

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
    }

    void Shader::setBool(const string& name, bool value)
    {
        backend->setBool(name, value);
    }

    void Shader::setInt(const string& name, int value)
    {
        backend->setInt(name, value);
    }

    void Shader::setFloat(const string& name, float value)
    {
        backend->setFloat(name, value);
    }

    void Shader::setVec2(const string& name, const vec2& value)
    {
        backend->setVec2(name, value);
    }

    void Shader::setVec2(const string& name, float x, float y)
    {
        backend->setVec2(name, {x, y});
    }

    void Shader::setVec3(const string& name, const vec3& value)
    {
        backend->setVec3(name, value);
    }

    void Shader::setVec3(const string& name, float x, float y, float z)
    {
        backend->setVec3(name, {x, y, z});
    }

    void Shader::setVec4(const string& name, const vec4& value)
    {
        backend->setVec4(name, value);
    }

    void Shader::setVec4(const string& name, float x, float y, float z, float w)
    {
        backend->setVec4(name, {x, y, z, w});
    }

    void Shader::setMat4(const string& name, const mat4& mat)
    {
        backend->setMat4(name, mat);
    }

    void Shader::use()
    {
        backend->use();
    }

    void Shader::unuse()
    {
        backend->unuse();
    }

    bool Shader::isValid () const
    {
        return backend->isValid();
    }

}
