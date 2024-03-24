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

#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(mesh->m_data.vao);



    }

    void RenderModule::refreshMesh(Mesh* mesh)
    {
        glBindBuffer(GL_ARRAY_BUFFER, mesh->m_data.vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(mesh->m_vertices), mesh->m_vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        glBindVertexArray(0);

    }

    void RenderModule::renderMesh(Mesh* mesh, Shader* shader)
    {

        // Can't be removed?
        this->refreshMesh(mesh);


        // shader->m_shaderData;

        // glUseProgram(shaderProgram);
        // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glBindVertexArray(mesh->m_data.vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void RenderModule::initShader(Shader*)
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
        auto vertexShaderSource = loadFile("C:\\Users\\j.iznardo\\Documents\\Aiko\\assets\\shaders\\aiko.vs");
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

        auto fragmentShaderSource = loadFile("C:\\Users\\j.iznardo\\Documents\\Aiko\\assets\\shaders\\aiko.fs");
        char* tmp2 = fragmentShaderSource.data();
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &tmp2, NULL);
        glCompileShader(fragmentShader);
        // check for shader compile errors
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
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
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
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

        ivec2 screenSize = getDisplaySize();
        vec2 targetSize = { static_cast<float>(m_renderTexture2D.texture.width), static_cast<float>(-m_renderTexture2D.texture.height) };
        vec2 targScreen = { static_cast<float>(screenSize.x), static_cast<float>(screenSize.y) };

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

}

