#include "bgfx_shader_impl.h"

#include <stdexcept>
#include <algorithm>

#include <logger/logger.h>
#include <core/file.h>

#include "constants.h"

namespace aiko::bgfx
{

    BgfxShaderImpl::BgfxShaderImpl()
        : m_programHandle({::bgfx::kInvalidHandle})
    {
    }

    void BgfxShaderImpl::load(const char* vs, const char* fs)
    {


        auto getShaderDir = []() -> std::string
        {

            std::string base = global::GLOBAL_ASSET_PATH + "/build/shaders/";

            switch (::bgfx::getRendererType())
            {
                case ::bgfx::RendererType::Direct3D11:
                case ::bgfx::RendererType::Direct3D12:  base += "dx11/"; break;
                case ::bgfx::RendererType::Metal:       base += "metal/"; break;
                case ::bgfx::RendererType::OpenGL:      base += "glsl/"; break;
                case ::bgfx::RendererType::OpenGLES:    base += "essl/"; break;
                case ::bgfx::RendererType::Vulkan:      base += "spirv/"; break;
                case ::bgfx::RendererType::Agc:
                case ::bgfx::RendererType::Gnm:         base += "pssl/"; break;
                case ::bgfx::RendererType::Nvn:         base += "nvn/"; break;
                case ::bgfx::RendererType::Noop:
                case ::bgfx::RendererType::Count:
                default:
                    throw std::runtime_error("Unsupported bgfx renderer or Noop.");
            }

            return base;
        };

        std::string dir = getShaderDir();
        std::filesystem::path vshaderPath = dir + vs + std::string(".bin");
        std::filesystem::path fshaderPath = dir + fs + std::string(".bin");

        AIKO_ASSERT(std::filesystem::exists(vshaderPath), "Vertex shader file not found!");
        AIKO_ASSERT(std::filesystem::exists(fshaderPath), "Fragment shader file not found!");

        const auto vs_bytes = files::readFileBytes(vshaderPath.string().c_str());
        const auto fs_bytes = files::readFileBytes(fshaderPath.string().c_str());

        const auto vs_mem = ::bgfx::makeRef(vs_bytes.data(), vs_bytes.size());
        const auto fs_mem = ::bgfx::makeRef(fs_bytes.data(), fs_bytes.size());

        ::bgfx::ShaderHandle vsh = ::bgfx::createShader(vs_mem);
        ::bgfx::ShaderHandle fsh = ::bgfx::createShader(fs_mem);

        AIKO_ASSERT(::bgfx::isValid(vsh) == false, "Failed to create vertex shaders!");
        AIKO_ASSERT(::bgfx::isValid(fsh) == false, "Failed to create fragment shaders!");

        m_programHandle = ::bgfx::createProgram(vsh, fsh, true);

        AIKO_ASSERT( ::bgfx::isValid(m_programHandle) == false, "Failed to create fragment shaders!");

        {
            auto dumpUniforms = [&](::bgfx::ShaderHandle shader, const char* str)
            {

                constexpr const size_t MAX_UNIFORMS = 32;

                ::bgfx::UniformHandle uniforms[MAX_UNIFORMS];
                uint16_t count = ::bgfx::getShaderUniforms(shader, uniforms, MAX_UNIFORMS);

                for (uint16_t i = 0; i < count; ++i)
                {
                    ::bgfx::UniformInfo info;
                    ::bgfx::getUniformInfo(uniforms[i], info);

                    logger::Log::info("[%d] %s  -->  %s (type=%d, num=%d)", id(), str, info.name, info.type, info.num);

                    ::bgfx::UniformType::Enum type = info.type;

                    ::bgfx::UniformHandle u = ::bgfx::createUniform(info.name, type);

                    m_uniforms.insert(std::make_pair(info.name, u));

                }
            };
            dumpUniforms(vsh, vs);  // Vertex shader uniforms
            dumpUniforms(fsh, fs);  // Fragment shader uniforms
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
        m_programHandle = {::bgfx::kInvalidHandle};
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

        auto found = std::find_if(m_uniforms.begin(), m_uniforms.end(),
            [name](const std::pair<string, ::bgfx::UniformHandle> & t) -> bool
            {
                return t.first == name;
            });

        if (found != m_uniforms.end())
        {
            return found->second;
        }
        logger::Log::error( "UniformNotLoaded %s", name.c_str());
        AIKO_ASSERT(false, "Uniform not loaded");
        return {::bgfx::kInvalidHandle};
    }

    void BgfxShaderImpl::setShaderUniform(string name, vec4 value)
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

    ::bgfx::UniformHandle BgfxShaderImpl::getUniformHandle(const string &name)
    {
        return m_uniforms[name];
    }

}
