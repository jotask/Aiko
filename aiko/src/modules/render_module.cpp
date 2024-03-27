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

    RenderModule::RenderModule()
        : m_displayModule(nullptr)
        , m_scale(false)
        , m_renderTexture2D()
    {
    
    }

    RenderModule::~RenderModule()
    {
        glfwTerminate();
    }

    void RenderModule::connect(ModuleConnector* moduleConnector)
    {
        BIND_MODULE_REQUIRED(DisplayModule, moduleConnector, m_displayModule)
        BIND_MODULE_REQUIRED(CameraModule, moduleConnector, m_cameraModule)
    }

    void RenderModule::preInit()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }
    
    void RenderModule::init()
    {

        if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == false)
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return;
        }

        glViewport(0, 0, 800, 600);

        auto size = getDisplaySize();
        EventSystem::it().bind<WindowResizeEvent>(this, &RenderModule::onWindowResize);
    }

    static Shader* shader;

    void RenderModule::initMesh( Mesh* mesh )
    {
        glGenVertexArrays(1, &mesh->m_data.vao);
        glGenBuffers(1, &mesh->m_data.vbo);
        glGenBuffers(1, &mesh->m_data.ebo);
        refreshMesh( mesh );
    }

    void RenderModule::refreshMesh(Mesh* mesh)
    {

        glBindVertexArray(mesh->m_data.vao);

        glBindBuffer(GL_ARRAY_BUFFER, mesh->m_data.vbo);
        glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(float), mesh->m_vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->m_data.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_indices.size() * sizeof(unsigned int), mesh->m_indices.data(), GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

    }

    void RenderModule::renderMesh(Transform* transform, Mesh* mesh, Shader* shader)
    {

        // Can be removed?
        this->refreshMesh(mesh);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shader->m_shaderData.id);

        unsigned int transformLoc = glGetUniformLocation(shader->m_shaderData.id, "transform");
        glm::mat4 matrix = glm::mat4(1.0f); // transform->getMatrix();
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matrix));

        glBindVertexArray(mesh->m_data.vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

    }

    void RenderModule::initShader(Shader* shader, const char* vsPath, const char* fsPath)
    {

        auto loadFile = [](std::string filename) -> std::string
        {
            std::ifstream file(filename, std::ios_base::binary | std::ios_base::in);
            if (!file.is_open())
            {
                throw std::runtime_error("Failed to open " + filename);
            }
            using Iterator = std::istreambuf_iterator<char>;
            std::string content(Iterator{ file }, Iterator{});
            if (!file)
            {
                throw std::runtime_error("Failed to read " + filename);
            }
            return content;
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
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        shader->m_shaderData.id = shaderProgram;

    }

    void RenderModule::refreshShader(Mesh*)
    {

    }

    void RenderModule::postInit()
    {

    }
    
    void RenderModule::preUpdate()
    {

    }
    
    void RenderModule::update()
    {

    }
    
    void RenderModule::postUpdate()
    {

    }
    
    void RenderModule::preRender()
    {

    }
    
    void RenderModule::render()
    {

    }
    
    void RenderModule::postRender()
    {

    }
    
    void RenderModule::beginFrame()
    {
        clearBackground(BLACK);
    }
    
    void RenderModule::endFrame()
    {

        // ivec2 screenSize = getDisplaySize();
        // vec2 targetSize = { static_cast<float>(m_renderTexture2D.texture.width), static_cast<float>(-m_renderTexture2D.texture.height) };
        // vec2 targScreen = { static_cast<float>(screenSize.x), static_cast<float>(screenSize.y) };

        /*
        drawTexturePro(
            m_renderTexture2D.texture,
            Rectangle{ 0, 0, targetSize.x, targetSize.y },
            Rectangle{ 0, 0, targScreen.x, targScreen.y },
            { 0, 0 },
            0,
            WHITE
        );
        */

    }

    ivec2 RenderModule::getDisplaySize()
    {
        return m_displayModule->getCurrentDisplay().getDisplaySize();
    }

    texture::RenderTexture2D* RenderModule::getRenderTexture()
    {
        return &m_renderTexture2D;
    }

    void RenderModule::onWindowResize(Event& event)
    {
        const auto& msg = static_cast<const WindowResizeEvent&>(event);

        if (m_scale == true)
        {
            return;
        }

        auto screenWidth = msg.width;
        auto screenHeight = msg.height;

        glViewport(0, 0, screenWidth, screenHeight);

    }

    void RenderModule::clearBackground(Color color)
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void RenderModule::beginMode2D(Camera* cam)
    {

    }

    void RenderModule::endMode2D()
    {

    }

    void RenderModule::beginMode3D(Camera* cam)
    {

    }

    void RenderModule::endMode3D()
    {

    }

    void RenderModule::beginTextureMode(texture::RenderTexture2D& target)
    {

    }

    void RenderModule::endTextureMode(void)
    {

    }

    void RenderModule::beginShaderMode(aiko::ShaderData* shader)
    {

    }

    void RenderModule::endShaderMode(void)
    {

    }

    void RenderModule::beginBlendMode(BlendMode mode)
    {

    }

    void RenderModule::endBlendMode(void)
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

