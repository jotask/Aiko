#pragma once

#include <aiko_types.h>
#include <math/math.h>

#include "types/render_types.h"

namespace aiko
{

    class Shader
    {
    public:

        Shader();
        ~Shader() = default;

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

        aiko::ShaderData* getData();

        bool isvalid() const { return isValid; }

    protected:
        virtual void connect();

    private:

        bool isValid = false;
        aiko::ShaderData m_shaderData;

    public:

        int getUniformLocation(const string& name);

    };
}
