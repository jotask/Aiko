#pragma once

#include <vector>
#include <functional>

#include "aiko_types.h"
#include "types/camera_types.h"
#include "types/render_types.h"
#include "types/textures.h"
#include "types/asset_type.h"
#include "shared/math.h"
#include "types/render_types.h"

namespace aiko
{
    class RenderModule;
    class RenderSystem;

    class Shader
    {
    public:
        friend class RenderModule;
        friend class RenderSystem;

        Shader();
        ~Shader() = default;

        void load(const char* vs, const char* fs);
        virtual void unload();

        // Type Helpers

        void setBool(const std::string& name, bool value) const;
        void setInt(const std::string& name, int value) const;
        void setFloat(const std::string& name, float value) const;
        void setVec2(const std::string& name, const vec2& value) const;
        void setVec2(const std::string& name, float x, float y) const;
        void setVec3(const std::string& name, const vec3& value) const;
        void setVec3(const std::string& name, float x, float y, float z) const;
        void setVec4(const std::string& name, const vec4& value) const;
        void setVec4(const std::string& name, float x, float y, float z, float w) const;
        void setMat4(const std::string& name, const mat4& mat) const;

        void use();
        void unuse();

        // void setShaderValue(int locIndex, const int& value);
        // void setShaderValue(int locIndex, const float& value);
        // void setShaderValue(int locIndex, const ivec2& value);
        // void setShaderValue(int locIndex, const vec2& value);
        // void setShaderValue(int locIndex, const vec3& value);
        // void setShaderValue(int locIndex, const vec4& value);
        // void setShaderValue(int locIndex, const std::vector<vec2>& value);

        aiko::ShaderData* getData();

        bool isvalid() const { return isValid; }

    protected:
        virtual void connect();

    private:

        bool isValid = false;
        aiko::ShaderData m_shaderData;

        using LoadShaderData = std::function<aiko::ShaderData(const char*, const char*)>;//  aiko::ShaderData(RenderModule::*)(const char*, const char*);
        using UnloadShaderData = std::function<void(aiko::ShaderData)>;

        LoadShaderData loadShaderData = nullptr;
        UnloadShaderData unloadShaderData = nullptr;

        int getUniformLocation(const std::string& name) const;

    };
}
