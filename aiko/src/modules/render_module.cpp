#include "modules/render_module.h"


#include <fstream>

#include <iostream>
#include <set>
#include <vector>

#include <algorithm>

#include "aiko.h"
#include "constants.h"
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
#include "constants.h"

#include "modules/render_primitives.h"

namespace aiko
{

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

            if (::aiko::global::GPU_LOGGING == false)
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

            string typeStr;

            switch (type)
            {
            case GL_DEBUG_TYPE_ERROR:               typeStr = "Error"; break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeStr = "Deprecated Behaviour;"; break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  typeStr = "Undefined Behaviour"; break;
            case GL_DEBUG_TYPE_PORTABILITY:         typeStr = "Portability"; break;
            case GL_DEBUG_TYPE_PERFORMANCE:         typeStr = "Performance"; break;
            case GL_DEBUG_TYPE_MARKER:              typeStr = "Marker"; break;
            case GL_DEBUG_TYPE_PUSH_GROUP:          typeStr = "Push Group"; break;
            case GL_DEBUG_TYPE_POP_GROUP:           typeStr = "Pop Group"; break;
            case GL_DEBUG_TYPE_OTHER:               return; // Log::error << "Type: Other"; break;
            }

            // fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            //     (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            //     type, severity, message);

            Log::error("GL::", typeStr, "::", message);

            int a = 0;

        }
    }

    RenderModule::RenderModule(Aiko* aiko)
        : BaseModule(aiko)
        , m_displayModule(nullptr)
        , m_scale(false)
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

    void RenderModule::init()
    {

        if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == false)
        {
            Log::error("Failed to initialize GLAD");
            return;
        }

        const AikoConfig cfg = getAiko()->getConfig();

        background_color = getAiko()->getConfig().background_color;
        glViewport(0, 0, cfg.width, cfg.height);
        glEnable(GL_CULL_FACE);

        // During init, enable debug output
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(opengl::onGlError, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

        m_passthrought = createShader();
        m_passthrought->load(
            global::getAssetPath("shaders/aiko_passthrought.vs").c_str(),
            global::getAssetPath("shaders/aiko_passthrought.fs").c_str()
        );

        m_passthrought->use();
        m_passthrought->setInt("screenTexture", 0);

        initScreenFbo();

        EventSystem::it().bind<WindowResizeEvent>(this, &RenderModule::onWindowResize);
    }

    void RenderModule::postInit()
    {
        if (gltInit() == false)
        {
            Log::error("glInit failed");
        }
        const AikoConfig cfg = getAiko()->getConfig();
        gltViewport(cfg.width, cfg.height);
    }

    void RenderModule::beginFrame()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_screenFbo.renderTexture.framebuffer);
        glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)
        clearBackground(background_color);
        // Set depth buffer to write mode
        glDepthMask(GL_TRUE);
    }

    void RenderModule::endFrame()
    {
        // Bind back to default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        clearBackground(BLACK);

        // Get window size
        auto window_size = m_displayModule->getCurrentDisplay().getDisplaySize();
        glViewport(0, 0, window_size.x, window_size.y);

        if (getAiko()->getConfig().auto_render == false)
        {
            return;
        }

        m_passthrought->use();
        m_passthrought->setInt("screenTexture", 0);
        glBindVertexArray(m_screenFbo.vao);
        glBindTexture(GL_TEXTURE_2D, m_screenFbo.renderTexture.texture.id);	// use the color attachment texture as the texture of the quad plane
        glDrawArrays(GL_TRIANGLES, 0, 6);
        m_passthrought->unuse();

    }

    void RenderModule::drawText(string texto, float x, float y , float scale, Color color)
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

            auto allCharSupported = [](string str) -> bool
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
        return &m_screenFbo.renderTexture;
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
            m_screenFbo.renderTexture.texture.width = screenWidth;
            m_screenFbo.renderTexture.texture.height = screenHeight;
            m_screenFbo.renderTexture.depth.width = screenWidth;
            m_screenFbo.renderTexture.depth.height = screenHeight;
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
        auto texture = m_screenFbo.renderTexture.texture;

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
        glBindTexture(GL_TEXTURE_2D, texture.id);
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
        glBindTexture(GL_TEXTURE_2D, m_screenFbo.renderTexture.texture.id);
    }

    void RenderModule::beginTextureMode(texture::RenderTexture2D& target)
    {
        glBindTexture(GL_TEXTURE_2D, target.texture.id);
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

