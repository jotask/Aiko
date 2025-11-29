#pragma once

namespace aiko
{
    namespace interfaces
    {
        struct IShaderImpl
        {
            virtual ~IShaderImpl() = default;

            virtual void use() = 0;
            virtual void unuse() = 0;
            virtual bool isValid() const = 0;
            virtual uint id() const = 0;

            // load
            virtual void load(const char*, const char*) = 0;
            virtual void unload() = 0;

            // Uniforms
            virtual void setBool(const string& name, bool value) = 0;
            virtual void setInt(const string& name, int value) = 0;
            virtual void setFloat(const string& name, float value) = 0;
            virtual void setVec2(const string& name, const vec2& value) = 0;
            virtual void setVec3(const string& name, const vec3& value) = 0;
            virtual void setVec4(const string& name, const vec4& value) = 0;
            virtual void setMat4(const string& name, const mat4& mat) = 0;

        };
    }
}
