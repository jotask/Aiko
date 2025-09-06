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

        // AIKO_DISABLE_CODE
        {

            auto generateShaderAssetGlobalPath = [](const char* shader) -> auto
                {
                    std::filesystem::path filePath = ::aiko::global::GLOBAL_ASSET_PATH;
                    filePath /= "build/shaders";
                    filePath /= std::string(shader) + ".bin";
                    assert(std::filesystem::exists(filePath) && "Shader doesn't exist");
                    return filePath;

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

            auto mem = ::bgfx::makeRef(v_shader_data.data(), uint32_t(v_shader_data.size()));
            printf("vsh magic: %02X %02X %02X %02X\n", mem->data[0], mem->data[1], mem->data[2], mem->data[3]);

            ::bgfx::ShaderHandle vsh = ::bgfx::createShader(
                ::bgfx::makeRef(v_shader_data.data(), static_cast<uint32_t>(v_shader_data.size()))
            );

            ::bgfx::ShaderHandle fsh = ::bgfx::createShader(
                ::bgfx::makeRef(f_shader_data.data(), static_cast<uint32_t>(f_shader_data.size()))
            );

            if (::bgfx::isValid(vsh) == false || ::bgfx::isValid(fsh) == false)
            {
                std::exit(9);
            }


            ::bgfx::ProgramHandle program = ::bgfx::createProgram(vsh, fsh, true);

            ShaderData data = { 0 };
            data.id = program.idx;

            return data;
        }

        return { 0 };

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