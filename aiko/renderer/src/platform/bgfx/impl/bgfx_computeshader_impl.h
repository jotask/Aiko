#pragma once

#include <map>

#include <aiko_types.h>
#include <bgfx/bgfx.h>

#include "interfaces/icomputeshader_impl.h"

namespace aiko::renderer::bgfx
{

    class BgfxComputeShaderImpl : public interfaces::IComputeShaderImpl
    {
    public:

        BgfxComputeShaderImpl() = default;
        virtual ~BgfxComputeShaderImpl() override = default;

        virtual bool isValid() const override;

        // load
        virtual void load(string file) override;
        virtual void unload() override;

        virtual uint id() override;

        ::bgfx::ProgramHandle getProgramHandler() const { return m_programHandle; }

    private:

        ::bgfx::ProgramHandle m_programHandle;
        std::map<string, ::bgfx::UniformHandle> m_uniforms;

    };
}
