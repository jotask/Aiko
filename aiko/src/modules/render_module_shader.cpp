#include "modules/render_module.h"

#include <string>
#include <fstream>

#include <core/libs.h>

#include "modules/module_connector.h"
#include "modules/display_module.h"
#include "modules/camera_module.h"
#include "models/camera.h"
#include "types/textures.h"
#include "events/events.hpp"
#include "types/render_types.h"
#include "models/mesh.h"
#include "models/shader.h"
#include "components/transform_component.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace aiko
{

    static Shader* shader;

    void RenderModule::initShader(Shader* shader, const char* vsPath, const char* fsPath)
    {

        auto loadFile = [](std::string filename) -> std::string
        {
            try
            {
                std::ifstream vShaderFile;
                vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
                vShaderFile.open(filename);
                std::stringstream vShaderStream;
                vShaderStream << vShaderFile.rdbuf();
                vShaderFile.close();
                return vShaderStream.str();
            }
            catch (std::ifstream::failure& e)
            {
                std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
            }
            return "";
        };

        // build and compile our shader program
        // ------------------------------------

        // vertex shader
        auto vertexShaderSource = loadFile(vsPath);
        char* tmp1 = vertexShaderSource.data();
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &tmp1, NULL);
        glCompileShader(vertexShader);

        // check for shader compile errors
        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
            int error = -1;
        }

        // fragment shader
        auto fragmentShaderSource = loadFile(fsPath);
        char* tmp2 = fragmentShaderSource.data();
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &tmp2, NULL);
        glCompileShader(fragmentShader);

        // check for shader compile errors
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (success == false)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
            int error = -1;
        }

        // link shaders
        unsigned int shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        // check for linking errors
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (success == false) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
            int error = -1;
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        shader->m_shaderData.id = shaderProgram;

    }

    void RenderModule::refreshShader(Mesh*)
    {

    }

    aiko::ShaderData RenderModule::loadShaderData(const char* vs, const char* fs)
    {
        return {};
    }

    void RenderModule::unloadShader(aiko::ShaderData& data)
    {

    }

    int RenderModule::getShaderLocation(aiko::ShaderData& shader, const char* uniformName)
    {
        return -1;
    }

    void RenderModule::setShaderUniformValue(aiko::ShaderData& shader, int locIndex, const void* value, aiko::ShaderUniformDataType uniformType)
    {

    }

    void RenderModule::setShaderUniformValueV(aiko::ShaderData& shader, int locIndex, const void* value, aiko::ShaderUniformDataType uniformType, int count)
    {

    }

}

