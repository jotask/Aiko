#include "bgfx_computeshader_impl.h"

#include <cstring>

#include <logger/logger.h>
#include <core/file.h>

#include "platform/bgfx/bgfx_platform_helper.h"

namespace aiko::renderer::bgfx
{

    bool BgfxComputeShaderImpl::isValid() const
    {
        return ::bgfx::isValid(m_programHandle);
    }

    void BgfxComputeShaderImpl::load(string file)
    {
        std::string dir = getShaderRootDir();
        std::filesystem::path cshaderPath = dir + file + std::string(".bin");

        AIKO_ASSERT(std::filesystem::exists(cshaderPath), "Compute shader file not found!");

        const auto cs_bytes = files::readFileBytes(cshaderPath.string().c_str());

        AIKO_ASSERT(cs_bytes.empty() == false, "Compute shader is empty!");

        const ::bgfx::Memory* cs_mem = ::bgfx::alloc(static_cast<uint32_t>(cs_bytes.size()));
        std::memcpy(cs_mem->data, cs_bytes.data(), cs_bytes.size());

        ::bgfx::ShaderHandle csh = ::bgfx::createShader(cs_mem);
        AIKO_ASSERT(::bgfx::isValid(csh), "Failed to create compute shader!");

        m_programHandle = ::bgfx::createProgram(csh, true);

        AIKO_ASSERT( ::bgfx::isValid(m_programHandle), "Failed to create compute shader!");

        {
            dumpShaderUniforms(csh, m_uniforms, file.c_str());
        }

        if(m_uniforms.size() == 0)
        {
            logger::Log::info("[%d] %s --> %s",id(), file.c_str(), "Shader with no uniforms");
        }
    }

    void BgfxComputeShaderImpl::unload()
    {
    }

    uint BgfxComputeShaderImpl::id()
    {
        return m_programHandle.idx;
    }
}
