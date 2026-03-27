#pragma once

#include <map>

#include <aiko_types.h>
#include <math/math_vector.h>
#include <bgfx/bgfx.h>

#include "interfaces/ishader_impl.h"

namespace aiko::renderer::bgfx
{

    class BgfxShaderImpl : public interfaces::IShaderImpl
    {
    public:

        BgfxShaderImpl();
        virtual ~BgfxShaderImpl() override = default;

        virtual uint id() const override;
        virtual void use() override;
        virtual void unuse() override;
        virtual bool isValid() const override;

        virtual void load(const char*, const char*) override;
        virtual void unload() override;

        virtual void setBool(const string& name, bool value) override;
        virtual void setInt(const string& name, int value) override;
        virtual void setFloat(const string& name, float value) override;
        virtual void setVec2(const string& name, const vec2& value) override;
        virtual void setVec3(const string& name, const vec3& value) override;
        virtual void setVec4(const string& name, const vec4& value) override;
        virtual void setMat4(const string& name, const mat4& mat) override;

        virtual void setVec4Array(const string& name, const vec4* values, uint32_t count) override;

        ::bgfx::ProgramHandle getProgramHandler() const { return m_programHandle; }

        ::bgfx::UniformHandle getUniformHandle(const string& name);
        bool hasUniform(const string& name) const;

    private:

        string vertex_file;
        string fragment_file;

        ::bgfx::ProgramHandle m_programHandle;
        std::map<string, ::bgfx::UniformHandle> m_uniforms;

        ::bgfx::UniformHandle getUniformLocation(const string& name);
        void setShaderUniform(const string& name, vec4 value);


    };
}
