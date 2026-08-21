#pragma once

#include <unordered_map>

#include <aiko_types.h>
#include <bgfx/bgfx.h>

#include "interfaces/icomputeshader_impl.h"

namespace aiko::renderer::bgfx
{

    class BgfxComputeShaderImpl : public interfaces::IComputeShaderImpl
    {
    public:

        BgfxComputeShaderImpl();
        virtual ~BgfxComputeShaderImpl() override = default;

        virtual bool isValid() const override;

        // load
        virtual void load(string file) override;
        virtual void unload() override;

        virtual uint id() const override;

        ::bgfx::ProgramHandle getProgramHandler() const { return m_programHandle; }

        ::bgfx::UniformHandle getUniformHandle(const string& name);

    private:

        string m_file;
        ::bgfx::ProgramHandle m_programHandle;
        std::unordered_map<string, ::bgfx::UniformHandle> m_uniforms;

    };
}
