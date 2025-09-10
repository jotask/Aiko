#ifdef AIKO_NATIVE

#include "opengl_render_module.h"

#include <fstream>

#include <core/libs.h>

#include "modules/module_connector.h"
#include "modules/display_module.h"
#include "models/camera.h"
#include "types/textures.h"
#include "events/events.hpp"
#include "types/render_types.h"
#include "models/mesh.h"
#include "models/shader.h"
#include "components/transform_component.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace aiko::native
{

    void OpenglRenderModule::refreshShader(Shader*)
    {

    }

    aiko::ShaderData OpenglRenderModule::loadShaderData(const char* vertex, const char* fragment)
    {
        auto loadFile = [](const char* filename) -> string
            {

                assert(filename != nullptr && "Shader file name can't be nullptr");

                try
                {
                    std::ifstream shaderFile;
                    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
                    shaderFile.open(filename);
                    std::stringstream shaderStream;
                    shaderStream << shaderFile.rdbuf();
                    shaderFile.close();
                    return shaderStream.str();
                }
                catch (std::ifstream::failure& e)
                {
                    Log::error("SHADER::FILE_NOT_SUCCESSFULLY_READ: ", e.what());
                    assert(false);
                }
                return "";
            };
        aiko::string vs = loadFile(vertex);
        aiko::string fs = loadFile(fragment);
        aiko::ShaderData shader = loadShaderSrc(vs.data(), fs.data());
        return shader;
    }

    aiko::ShaderData OpenglRenderModule::loadShaderSrc(const char* vertex, const char* fragment)
    {

        aiko::ShaderData shader;

        int success = 0;
        char infoLog[512];

        auto loadShader = [&](const char* source, GLenum shaderType)
        {
            const GLuint id = glCreateShader(shaderType);

            glShaderSource(id, 1, &source, NULL);
            glCompileShader(id);

            glGetShaderiv(id, GL_COMPILE_STATUS, &success);

            if (!success)
            {
                glGetShaderInfoLog(id, 512, NULL, infoLog);
                Log::error("SHADER::COMPILATION_FAILED", infoLog);
                assert(false);
            }

            return id;
        };

        const GLuint vertexShader = loadShader(vertex, GL_VERTEX_SHADER);
        const GLuint fragmentShader = loadShader(fragment, GL_FRAGMENT_SHADER);

        // link shaders
        const GLuint shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        // check for linking errors
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (success == false)
        {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            Log::error("SHADER::PROGRAM::LINKING_FAILED", infoLog);
            assert(false);
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        shader.id = shaderProgram;

        // TODO Do I need to store the other attributeS?
        // https://stackoverflow.com/questions/440144/in-opengl-is-there-a-way-to-get-a-list-of-all-uniforms-attribs-used-by-a-shade
        // 
        // Set Uniforms
        {

            GLint count;

            GLint size; // size of the variable
            GLenum type; // type of the variable (float, vec3 or mat4, etc)

            const GLsizei bufSize = 128; // maximum name length
            GLchar name[bufSize]; // variable name in GLSL
            GLsizei length; // name length

            glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORMS, &count);
            // printf("Active Uniforms: %d\n", count);

            for (GLint i = 0; i < count; i++)
            {
                glGetActiveUniform(shaderProgram, (GLuint)i, bufSize, &length, &size, &type, name);
                shader.locs.emplace(name, i);
                // printf("Uniform #%d Type: %u Name: %s\n", i, type, name);
            }
        }

        return shader;

    }

    int OpenglRenderModule::loadShaderUniform(Shader* shader, const string& name, ShaderUniformDataType type)
    {
        AIKO_DEBUG_BREAK
        return { 0 };
    }

    void OpenglRenderModule::unloadShader(aiko::ShaderData& data)
    {
        AIKO_DEBUG_BREAK
    }

    void OpenglRenderModule::setShaderUniform(Shader*, string name, vec4 value)
    {
        AIKO_DEBUG_BREAK
    }


}
#endif