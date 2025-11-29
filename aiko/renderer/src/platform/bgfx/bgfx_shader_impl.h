#pragma once

#include <map>

#include <aiko_types.h>
#include <math/math_vector.h>
#include <bgfx/bgfx.h>

#include "interfaces/ishader_impl.h"

namespace aiko::bgfx
{

    class BgfxShaderImpl : public interfaces::IShaderImpl
    {
    public:

        BgfxShaderImpl();
        virtual ~BgfxShaderImpl() = default;

        virtual void load(const char*, const char*) override;

        virtual void use() override;
        virtual void unuse() override;
        virtual bool isValid() const override;

        virtual void setBool(const string& name, bool value) override;
        virtual void setInt(const string& name, int value) override;
        virtual void setFloat(const string& name, float value) override;
        virtual void setVec2(const string& name, const vec2& value) override;
        virtual void setVec3(const string& name, const vec3& value) override;
        virtual void setVec4(const string& name, const vec4& value) override;
        virtual void setMat4(const string& name, const mat4& mat) override;

    private:

        ::bgfx::ProgramHandle m_programHandle;
        std::map<string, ::bgfx::UniformHandle> m_uniforms;

        ::bgfx::UniformHandle getUniformLocation(const string& name);
        void setShaderUniform(string name, vec4 value);


    };
}
