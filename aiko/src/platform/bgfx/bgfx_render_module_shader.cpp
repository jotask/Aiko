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

#include "bgfx_platform_helper.h"

namespace aiko::bgfx
{

    void BgfxRenderModule::refreshShader(Shader*)
    {

    }

    aiko::ShaderData BgfxRenderModule::loadShaderData(const char* vertex, const char* fragment)
    {
        auto getShaderDir = []() -> std::string
            {
                std::string base = ::aiko::global::GLOBAL_ASSET_PATH + "/build/shaders/";

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

        auto loadShader = [](const std::filesystem::path& path) -> const ::bgfx::Memory*
            {
                std::ifstream file(path, std::ios::binary | std::ios::ate);
                if (!file)
                {
                    throw std::runtime_error("Failed to open shader file: " + path.string());
                }

                std::streamsize size = file.tellg();
                file.seekg(0, std::ios::beg);

                uint8_t* data = new uint8_t[size];
                if (!file.read(reinterpret_cast<char*>(data), size))
                {
                    delete[] data;
                    throw std::runtime_error("Failed to read shader file: " + path.string());
                }

                // bgfx takes ownership of memory via release callback
                return ::bgfx::makeRef(data, (uint32_t)size, [](void* ptr, void*) { delete[](uint8_t*)ptr; });
            };

        std::string dir = getShaderDir();
        std::filesystem::path vshaderPath = dir + vertex + std::string(".bin");
        std::filesystem::path fshaderPath = dir + fragment + std::string(".bin");

        assert(std::filesystem::exists(vshaderPath) && "Vertex shader file not found!");
        assert(std::filesystem::exists(fshaderPath) && "Fragment shader file not found!");

        const ::bgfx::Memory* vmem = loadShader(vshaderPath);
        const ::bgfx::Memory* fmem = loadShader(fshaderPath);

        // printf("vsh magic: %02X %02X %02X %02X\n", vmem->data[0], vmem->data[1], vmem->data[2], vmem->data[3]);
        // printf("fsh magic: %02X %02X %02X %02X\n", fmem->data[0], fmem->data[1], fmem->data[2], fmem->data[3]);

        ::bgfx::ShaderHandle vsh = ::bgfx::createShader(vmem);
        ::bgfx::ShaderHandle fsh = ::bgfx::createShader(fmem);

        if (::bgfx::isValid(vsh) == false)
        {
            throw std::runtime_error("Failed to create vertex shaders!");
        }

        if (::bgfx::isValid(fsh) == false)
        {
            throw std::runtime_error("Failed to create fragment shaders!");
        }

        ::bgfx::ProgramHandle program = ::bgfx::createProgram(vsh, fsh, true);

        ShaderData data = {};
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