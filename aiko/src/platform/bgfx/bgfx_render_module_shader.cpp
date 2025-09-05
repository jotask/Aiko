#ifdef AIKO_BGFX

#include "bgfx_render_module.h"

#include <fstream>
#include <filesystem>

#include <core/libs.h>

#include "modules/module_connector.h"
#include "modules/display_module.h"
#include "models/camera.h"
#include "types/textures.h"
#include "events/events.hpp"
#include "types/render_types.h"
#include "models/mesh.h"
#include "models/shader.h"
#include "constants.h"
#include "components/transform_component.h"

#include <bgfx/bgfx.h>
#include <bx/bx.h>
#include <bx/file.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace aiko::bgfx
{

    void BgfxRenderModule::refreshShader(Shader*)
    {

    }

    aiko::ShaderData BgfxRenderModule::loadShaderData(const char* vertex, const char* fragment)
    {

        auto generateShaderAssetGlobalPath = [](const char* shader) -> auto
            {
                ::bx::FilePath filePath(::aiko::global::GLOBAL_ASSET_PATH);
                filePath.join("build/shaders");

                if constexpr (false)
                {
                    switch (::bgfx::getRendererType())
                    {
                    case ::bgfx::RendererType::Noop:
                    case ::bgfx::RendererType::Direct3D11:
                    case ::bgfx::RendererType::Direct3D12: filePath.join("dx11");  break;
                    case ::bgfx::RendererType::Agc:
                    case ::bgfx::RendererType::Gnm:        filePath.join("pssl");  break;
                    case ::bgfx::RendererType::Metal:      filePath.join("metal"); break;
                    case ::bgfx::RendererType::Nvn:        filePath.join("nvn");   break;
                    case ::bgfx::RendererType::OpenGL:     filePath.join("glsl");  break;
                    case ::bgfx::RendererType::OpenGLES:   filePath.join("essl");  break;
                    case ::bgfx::RendererType::Vulkan:     filePath.join("spirv"); break;

                    case ::bgfx::RendererType::Count:
                        BX_ASSERT(false, "You should not be here!");
                        break;
                    }
                }

                std::string shader_file = std::string(shader) + ".bin";

                filePath.join(shader_file.c_str());
                
                std::string s(filePath.getCPtr());
                std::filesystem::path outputPath(s);
                assert(std::filesystem::exists(outputPath) && "Shader dont' exist");

                return outputPath;

            };

        auto load_shader = [](const std::filesystem::path& path) -> auto
            {
                std::ifstream file(path, std::ios::binary | std::ios::ate);
                if (!file)
                {
                    throw std::runtime_error("Failed to open shader file: " + path.string());
                }

                std::streamsize size = file.tellg();
                file.seekg(0, std::ios::beg);

                std::vector<uint8_t> buffer(size);
                if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
                {
                    throw std::runtime_error("Failed to read shader file: " + path.string());
                }

                return buffer;
            };

        auto v_shader_path = generateShaderAssetGlobalPath(vertex);
        auto f_shader_path = generateShaderAssetGlobalPath(fragment);

        auto v_shader_data = load_shader(v_shader_path);
        auto f_shader_data = load_shader(f_shader_path);

        ::bgfx::ShaderHandle vsh = ::bgfx::createShader(
            ::bgfx::makeRef(v_shader_data.data(), static_cast<uint32_t>(v_shader_data.size()))
        );

        ::bgfx::ShaderHandle fsh = ::bgfx::createShader(
            ::bgfx::makeRef(f_shader_data.data(), static_cast<uint32_t>(f_shader_data.size()))
        );

        ::bgfx::ProgramHandle program = ::bgfx::createProgram(vsh, fsh, true);

        ShaderData data = { 0 };
        data.id = program.idx;

        return data;
    }

    aiko::ShaderData BgfxRenderModule::loadShaderSrc(const char* vertex, const char* fragment)
    {
        AIKO_DEBUG_BREAK
        return { 0 };
    }

    void BgfxRenderModule::unloadShader(aiko::ShaderData& data)
    {
        AIKO_DEBUG_BREAK
    }

}
#endif