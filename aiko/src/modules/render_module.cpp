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

#include "modules/render_primitives.h"

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
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    }

    namespace opengl
    {
        void onGlError(GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar* message,
            const void* userParam)
        {
            /*
            Regarding the warning (not error). It just warns you that your buffer will be put in video
            memory since you're using GL_STATIC_DRAW for your buffer. It's actually more of a log and
            you can safely ignore it. 
            */
            constexpr GLuint error_131185 = 131185;

            if (id == error_131185)
            {
                return;
            }

            // fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            //     (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            //     type, severity, message);
            // std::cout << "ERROR::GL::ERROR:: " << message << std::endl;

            int a = 0;

        }
    }

    void RenderModule::init()
    {

        if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == false)
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return;
        }

        glViewport(0, 0, 800, 600);
        glEnable(GL_CULL_FACE);

        // During init, enable debug output
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(opengl::onGlError, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

        auto size = getDisplaySize();
        EventSystem::it().bind<WindowResizeEvent>(this, &RenderModule::onWindowResize);
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        beginMode3D();
    }

    void RenderModule::beginMode2D()
    {
        glDisable(GL_DEPTH_TEST);
    }

    void RenderModule::endMode2D()
    {

    }

    void RenderModule::beginMode3D()
    {
        glEnable(GL_DEPTH_TEST);
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

