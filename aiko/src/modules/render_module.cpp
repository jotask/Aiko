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
        glEnable(GL_CULL_FACE);

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

