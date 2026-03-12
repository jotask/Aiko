#pragma once

#include <aiko_types.h>
#include <math/math.h>

#include "interfaces/ishader_impl.h"

namespace aiko
{
    namespace renderer
    {
        class RendererFactory;
    }

    class Shader
    {
    public:

        friend class renderer::RendererFactory;

        // Copy
        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;

        // Move
        Shader(Shader&&) noexcept = delete;
        Shader& operator=(Shader&&) noexcept = delete;

        Shader();
        ~Shader() = default;

        void* getImpl() const { return backend.get(); }

        uint id() const;

        void load(const char* fileCodeName);
        void load(const char* vs, const char* fs);
        void loadFromSource(const char* vs, const char* fs);
        void unload();

        // Type Helpers

        void setBool(const string& name, bool value);
        void setInt(const string& name, int value);
        void setFloat(const string& name, float value);
        void setVec2(const string& name, const vec2& value);
        void setVec2(const string& name, float x, float y);
        void setVec3(const string& name, const vec3& value);
        void setVec3(const string& name, float x, float y, float z);
        void setVec4(const string& name, const vec4& value);
        void setVec4(const string& name, float x, float y, float z, float w);
        void setMat4(const string& name, const mat4& mat);

        void use();
        void unuse();

        bool isValid() const;

    private:

        AikoPtr<interfaces::IShaderImpl> backend;

    };
}
