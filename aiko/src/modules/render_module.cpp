#include "modules/render_module.h"

#include <string>
#include <fstream>

#include <iostream>
#include <set>
#include <vector>
#include <string>
#include <algorithm>

#include "modules/module_connector.h"
#include "modules/display_module.h"
#include "models/camera.h"
#include "types/textures.h"
#include "events/events.hpp"
#include "types/render_types.h"
#include "models/mesh.h"
#include "models/shader.h"
#include "components/transform_component.h"
#include "render_primitives.h"
#include "core/libs.h"
#include "core/log.h"

#include "modules/render_primitives.h"

namespace aiko
{

    constexpr bool s_enableDebugLogs = false;

    RenderModule::RenderModule()
        : m_displayModule(nullptr)
        , m_scale(false)
        , m_renderTexture2D()
    {
    
    }

    RenderModule::~RenderModule()
    {
        
    }

    void RenderModule::connect(ModuleConnector* moduleConnector)
    {
        BIND_MODULE_REQUIRED(DisplayModule, moduleConnector, m_displayModule)
    }

    void RenderModule::preInit()
    {
        if (glfwInit() == false)
        {
            Log::error("Couldn't init GLFW");
        }
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

            if (s_enableDebugLogs == false)
            {
                return;
            }

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
                case GL_DEBUG_TYPE_ERROR:               Log::error( "Type: Error" ); break;
                case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: Log::error( "Type: Deprecated Behaviour" ); break;
                case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  Log::error( "Type: Undefined Behaviour" ); break;
                case GL_DEBUG_TYPE_PORTABILITY:         Log::error( "Type: Portability" ); break;
                case GL_DEBUG_TYPE_PERFORMANCE:         Log::error( "Type: Performance" ); break;
                case GL_DEBUG_TYPE_MARKER:              Log::error( "Type: Marker" ); break;
                case GL_DEBUG_TYPE_PUSH_GROUP:          Log::error( "Type: Push Group" ); break;
                case GL_DEBUG_TYPE_POP_GROUP:           Log::error( "Type: Pop Group" ); break;
                case GL_DEBUG_TYPE_OTHER:               return; // Log::error << "Type: Other"; break;
            }

            // fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            //     (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            //     type, severity, message);
            

            Log::error("GL::ERROR::", message);

            int a = 0;

        }
    }

    void RenderModule::init()
    {

        if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == false)
        {
            Log::error("Failed to initialize GLAD");
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
        m_renderTexture2D = createRenderTexture();

        EventSystem::it().bind<WindowResizeEvent>(this, &RenderModule::onWindowResize);
    }

    void RenderModule::postInit()
    {
        if (gltInit() == false)
        {
            Log::error("glInit failed");
        }
        gltViewport(800, 600);
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

        // bind to framebuffer and draw scene as we normally would to color texture 
        glBindFramebuffer(GL_FRAMEBUFFER, m_renderTexture2D.framebuffer);
        glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

        clearBackground({0.2f, 0.3f, 0.3f, 1.0f});

        // Set depth buffer to write mode
        glDepthMask(GL_TRUE);

    }
    
    void RenderModule::endFrame()
    {

        // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        clearBackground(BLACK);
        glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.

        // Render using our full screen shader program
        m_passthrought->use();

        // Bind the output texture from the previous shader program.
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_renderTexture2D.texture);
        m_passthrought->setInt("screenTexture", 0);

        // Draw three "vertices" as a triangle. (no buffers required)
        // At this point you should look at the contents of the vertex and fragment shaders.
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Unbind everything we were using.
        glBindTexture(GL_TEXTURE_2D, 0);
        m_passthrought->unuse();

    }

    void RenderModule::drawText(std::string texto, float x, float y , float scale, Color color)
    {
        gltBeginDraw();

        GLTtext* text1 = gltCreateText();
        if (gltSetText(text1, texto.c_str()) == GL_FALSE)
        {
            Log::error("glText can't set text");
            return;
        }

        auto size = getDisplaySize();

        gltColor(1.0f, 1.0f, 1.0f, 1.0f);

        {

            auto allCharSupported = [](std::string str) -> bool
            {
                for (char c : str)
                {
                    if (isspace(c))
                    {
                        return true;
                    }
                    if (gltIsCharacterDrawable(c) == GL_FALSE)
                    {
                        return false;
                    }
                }
                return true;
            };

            if (allCharSupported(texto) == false)
            {
                Log::warning("char not supported by font");
            }
        }

        gltDrawText2D(text1, x, y, scale );

        gltEndDraw();

        gltDeleteText(text1);

    }

    void RenderModule::dispose()
    {
        gltTerminate();
        glfwTerminate();
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
        gltViewport(screenWidth, screenHeight);

        // resize render texture
        {
            m_renderTexture2D.width = screenWidth;
            m_renderTexture2D.height = screenHeight;
            // color buffer attachment here
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            // and depth buffer attachment
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
        }

    }

    void RenderModule::clearBackground(Color color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void RenderModule::beginMode2D()
    {
        glDisable(GL_DEPTH_TEST);
    }

    void RenderModule::endMode2D()
    {
        auto texture = m_renderTexture2D.texture;

        // Set up vertex data and buffers for a fullscreen quad
        constexpr float vertices[] = {
            // Positions    // Texture coordinates
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-left corner
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-right corner
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f, // top-right corner
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f  // top-left corner
        };
        constexpr unsigned int indices[] = {
            0, 1, 2, // first triangle
            0, 2, 3  // second triangle
        };

        GLuint VAO, VBO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // Texture coordinate attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Unbind VAO
        glBindVertexArray(0);

        // Render fullscreen quad
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Cleanup
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);

        glEnable(GL_CULL_FACE);
    }

    void RenderModule::beginMode3D()
    {
        glEnable(GL_DEPTH_TEST);
    }

    void RenderModule::endMode3D()
    {

    }

    void RenderModule::beginTextureMode()
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_renderTexture2D.texture);
    }

    void RenderModule::beginTextureMode(texture::RenderTexture2D& target)
    {
        glBindTexture(GL_TEXTURE_2D, target.texture);
    }

    void RenderModule::endTextureMode(void)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void RenderModule::beginShaderMode(aiko::Shader* shader)
    {
        glDisable(GL_CULL_FACE);
        shader->use();
    }

    void RenderModule::endShaderMode(void)
    {
        glUseProgram(0);
    }

    void RenderModule::beginBlendMode(BlendMode mode)
    {

    }

    void RenderModule::endBlendMode(void)
    {

    }

}

