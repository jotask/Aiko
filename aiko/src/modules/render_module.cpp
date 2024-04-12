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
#include "render_primitives.h"

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

            switch (type)
            {
                case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
                case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
                case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
                case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
                case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
                case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
                case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
                case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
                case GL_DEBUG_TYPE_OTHER:               return; // std::cout << "Type: Other"; break;
            } std::cout << std::endl;

            // fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            //     (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            //     type, severity, message);
            std::cout << "ERROR::GL::ERROR:: " << message << std::endl;

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

        m_passthrought = createShader();
        m_passthrought->load(
            "C:\\Users\\j.iznardo\\Documents\\Aiko\\assets\\shaders\\aiko_passthrought.vs",
            "C:\\Users\\j.iznardo\\Documents\\Aiko\\assets\\shaders\\aiko_passthrought.fs"
        );

        m_passthrought->use();
        m_passthrought->setInt("screenTexture", 0);

        m_texture = createTexture();
        m_renderTexture2D = createScreenTexture();

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
        // clearBackground(BLACK);
        // glBindFramebuffer(GL_FRAMEBUFFER, m_renderTexture2D.renderTexture.framebuffer);
        // glEnable(GL_DEPTH_TEST);
        // clearBackground(BLACK);

        // begin
        {

            // render
            // ------
            // bind to framebuffer and draw scene as we normally would to color texture 
            glBindFramebuffer(GL_FRAMEBUFFER, m_renderTexture2D.renderTexture.framebuffer);
            glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

            // make sure we clear the framebuffer's content
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Set depth buffer to write mode
            glDepthMask(GL_TRUE);

        }

    }
    
    void RenderModule::endFrame()
    {

        aiko::Primitives::drawRectangle(aiko::vec3(0.0f), aiko::vec3(4.0f));

        // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.

        // clear all relevant buffers
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
        glClear(GL_COLOR_BUFFER_BIT);

        m_passthrought->use();
        glBindTexture(GL_TEXTURE_2D, m_renderTexture2D.renderTexture.texture);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindVertexArray(m_renderTexture2D.vao);
        glBindTexture(GL_TEXTURE_2D, m_renderTexture2D.renderTexture.texture);	// use the color attachment texture as the texture of the quad plane
        glDrawArrays(GL_TRIANGLES, 0, 6);
        m_passthrought->unuse();

    }

    ivec2 RenderModule::getDisplaySize()
    {
        return m_displayModule->getCurrentDisplay().getDisplaySize();
    }

    texture::RenderTexture2D* RenderModule::getRenderTexture()
    {
        return &m_renderTexture2D.renderTexture;
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

        // resize render texture
        {
            m_renderTexture2D.renderTexture.width = screenWidth;
            m_renderTexture2D.renderTexture.height = screenHeight;
            // color buffer attachment here
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            // and depth buffer attachment
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
        }

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

