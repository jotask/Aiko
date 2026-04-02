#include "bgfx_shader_impl.h"

#include <algorithm>

#include <logger/logger.h>
#include <core/file.h>

#include "platform/bgfx/bgfx_platform_helper.h"
#include "platform/bgfx/bgfx_types.h"

namespace aiko::renderer::bgfx
{

    BgfxShaderImpl::BgfxShaderImpl()
        : m_programHandle(AIKO_INVALID_HANDLE)
    {
    }

    void BgfxShaderImpl::load(const char* vs, const char* fs)
    {

        vertex_file = string(vs);
        fragment_file = string(fs);

        std::string dir = getShaderRootDir();
        std::filesystem::path vshaderPath = dir + vs + std::string(".bin");
        std::filesystem::path fshaderPath = dir + fs + std::string(".bin");

        AIKO_ASSERT(std::filesystem::exists(vshaderPath), "Vertex shader file not found!");
        AIKO_ASSERT(std::filesystem::exists(fshaderPath), "Fragment shader file not found!");

        const auto vs_bytes = files::readFileBytes(vshaderPath.string().c_str());
        const auto fs_bytes = files::readFileBytes(fshaderPath.string().c_str());

        AIKO_ASSERT(vs_bytes.empty() == false, "Vertex shader is empty!");
        AIKO_ASSERT(fs_bytes.empty() == false, "Fragment shader is empty!");

        const ::bgfx::Memory* vs_mem = ::bgfx::alloc(static_cast<uint32_t>(vs_bytes.size()));
        std::memcpy(vs_mem->data, vs_bytes.data(), vs_bytes.size());

        const ::bgfx::Memory* fs_mem = ::bgfx::alloc(static_cast<uint32_t>(fs_bytes.size()));
        std::memcpy(fs_mem->data, fs_bytes.data(), fs_bytes.size());

        ::bgfx::ShaderHandle vsh = ::bgfx::createShader(vs_mem);
        ::bgfx::ShaderHandle fsh = ::bgfx::createShader(fs_mem);

        AIKO_ASSERT(::bgfx::isValid(vsh), "Failed to create vertex shaders!");
        AIKO_ASSERT(::bgfx::isValid(fsh), "Failed to create fragment shaders!");

        m_programHandle = ::bgfx::createProgram(vsh, fsh, true);

        AIKO_ASSERT( ::bgfx::isValid(m_programHandle), "Failed to create fragment shaders!");

        {
            dumpShaderUniforms(vsh, m_uniforms, vs);  // Vertex shader uniforms
            dumpShaderUniforms(fsh, m_uniforms, fs);  // Fragment shader uniforms
        }

        if(m_uniforms.size() == 0)
        {
            logger::Log::info("[%d] %s / %s --> %s",id(), vs, fs, "Shader with no uniforms");
        }

    }

    void BgfxShaderImpl::unload()
    {
        if (isValid() == false)
        {
            return;
        }
        ::bgfx::destroy(m_programHandle);
        m_programHandle = AIKO_INVALID_HANDLE;
    }


    uint BgfxShaderImpl::id() const
    {
        return m_programHandle.idx;
    }

    void BgfxShaderImpl::use()
    {
        // s_renderModule->beginShaderMode(this);
    }

    void BgfxShaderImpl::unuse()
    {
        // s_renderModule->endShaderMode();
    }

    bool BgfxShaderImpl::isValid() const
    {
        return ::bgfx::isValid(m_programHandle);
    }

    ::bgfx::UniformHandle BgfxShaderImpl::getUniformLocation(const string& name)
    {
        auto found = m_uniforms.find(name);
        if (found != m_uniforms.end())
        {
            return found->second;
        }
        logger::Log::error( "[%s/%s] UniformNotLoaded %s", vertex_file.c_str(), fragment_file.c_str(), name.c_str());
        AIKO_ASSERT(false, "Uniform not loaded");
        return AIKO_INVALID_HANDLE;
    }

    void BgfxShaderImpl::setShaderUniform(const string& name, vec4 value)
    {
        ::bgfx::UniformHandle loc = getUniformLocation(name);
        if (::bgfx::isValid(loc) == false)
        {
            logger::Log::error("Uniform not valid!");
            return;
        }
        float val[4] = { value.x , value.y , value.z , value.w };
        ::bgfx::setUniform(loc, &val);
    }

    void BgfxShaderImpl::setBool(const string& name, bool value)
    {
        setShaderUniform(name, value);
    }

    void BgfxShaderImpl::setInt(const string& name, int value)
    {
        setShaderUniform(name, value);
    }

    void BgfxShaderImpl::setFloat(const string& name, float value)
    {
        setShaderUniform(name, value);
    }

    void BgfxShaderImpl::setVec2(const string& name, const vec2& value)
    {
        const vec4 val = {value.x, value.y, 0.0f, 0.0f };
        setShaderUniform(name, val);
    }

    void BgfxShaderImpl::setVec3(const string& name, const vec3& value)
    {
        const vec4 val = {value.x, value.y, value.z, 0.0f };
        setShaderUniform(name, val);
    }

    void BgfxShaderImpl::setVec4(const string& name, const vec4& value)
    {
        setShaderUniform(name, value);
    }

    void BgfxShaderImpl::setMat4(const string& name, const mat4& mat)
    {
        AIKO_DEBUG_BREAK
    }

    void BgfxShaderImpl::setVec4Array(const string& name, const vec4* values, uint32_t count)
    {
        ::bgfx::UniformHandle loc = getUniformLocation(name);
        if (::bgfx::isValid(loc) == false || values == nullptr || count == 0)
        {
            return;
        }
        ::bgfx::setUniform(loc, values, count);
    }

    ::bgfx::UniformHandle BgfxShaderImpl::getUniformHandle(const string &name)
    {
        auto found = m_uniforms.find(name);
        if (found != m_uniforms.end())
        {
            return found->second;
        }
        logger::Log::error( "[%s/%s] UniformNotLoaded %s", vertex_file.c_str(), fragment_file.c_str(), name.c_str());
        return AIKO_INVALID_HANDLE;
    }

    bool BgfxShaderImpl::hasUniform(const string& name) const
    {
        auto it = m_uniforms.find(name);
        return it != m_uniforms.end();
    }
}
