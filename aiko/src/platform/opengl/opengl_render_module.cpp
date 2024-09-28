#ifdef AIKO_NATIVE

#include "opengl_render_module.h"

#include <fstream>

#include <iostream>
#include <set>
#include <vector>

#include <algorithm>

#include "aiko.h"
#include "constants.h"
#include "modules/module_connector.h"
#include "modules/display_module.h"
#include "types/textures.h"
#include "events/events.hpp"
#include "types/render_types.h"
#include "models/mesh.h"
#include "models/shader.h"
#include "components/transform_component.h"
#include "modules/render/render_primitives.h"
#include "core/libs.h"
#include "core/log.h"
#include "constants.h"

#include "platform/opengl/opengl_2d_renderer.h"
#include "platform/opengl/opengl_3d_renderer.h"

#include "modules/render/render_primitives.h"

namespace aiko::native
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

    OpenglRenderModule::OpenglRenderModule(Aiko* aiko)
        : RenderModule(aiko)
    {
    
    }

    OpenglRenderModule::~OpenglRenderModule()
    {
        
    }


    void OpenglRenderModule::preInit()
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

    void OpenglRenderModule::init()
    {

        RenderModule::init();

        if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == false)
        {
            Log::error("Failed to initialize GLAD");
            return;
        }

        const auto windows_size = getDisplaySize();
        glViewport(0, 0, windows_size.x, windows_size.y);
        glEnable(GL_CULL_FACE);

        // During init, enable debug output
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(opengl::onGlError, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

        // Init context
        {
            // TODO Extract this, into AikoCfg
            RenderModule::RenderersCtx& renderers = getRenderers();
            renderers.emplace(ContextType::Render2D, std::make_shared<Opengl2DRenderer>(this));
            renderers.emplace(ContextType::Render3D, std::make_shared<Opengl3DRenderer>(this));
        }

        m_passthrought = createShader();
        m_passthrought->load(
            aiko::global::getAssetPath("shaders/aiko_passthrought.vs").c_str(),
            aiko::global::getAssetPath("shaders/aiko_passthrought.fs").c_str()
        );

        m_passthrought->use();
        m_passthrought->setInt("screenTexture", 0);
        m_passthrought->unuse();

        initScreenFbo();

        Log::info() << "OpenGL Renderer : " << glGetString(GL_RENDERER);
        Log::info() << "OpenGL Vendor : " << glGetString(GL_VENDOR);
        Log::info() << "OpenGL Version : " << glGetString(GL_VERSION);
        Log::info() << "GLSL Version : " << glGetString(GL_SHADING_LANGUAGE_VERSION);
        Log::info() << "GLFW Version : " << GLFW_VERSION_MAJOR << "." << GLFW_VERSION_MINOR << "." << GLFW_VERSION_REVISION;

    }

    void OpenglRenderModule::beginFrame()
    {
        RenderModule::beginFrame();
        glBindFramebuffer(GL_FRAMEBUFFER, m_screenFbo.renderTexture.framebuffer);
        glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)
        clearBackground(background_color);
        // Set depth buffer to write mode
        glDepthMask(GL_TRUE);
    }

    void OpenglRenderModule::endFrame()
    {

        RenderModule::endFrame();

        // Bind back to default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        clearBackground(BLACK);

        // Get window size
        const ivec2 window_size = getDisplaySize();
        glViewport(0, 0, m_screenFbo.renderTexture.texture.width, m_screenFbo.renderTexture.texture.height);

        if (getAiko()->getConfig().auto_render == false)
        {
            return;
        }

        glBindVertexArray(m_screenFbo.vao);

        m_passthrought->use();
        m_passthrought->setInt("screenTexture", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_screenFbo.renderTexture.texture.id);	// use the color attachment texture as the texture of the quad plane

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        m_passthrought->unuse();

    }

    void OpenglRenderModule::drawText(string texto, float x, float y , float scale, Color color)
    {

    }

    void OpenglRenderModule::dispose()
    {
        RenderModule::dispose();
        glfwTerminate();
    }

    texture::RenderTexture2D* OpenglRenderModule::getRenderTexture()
    {
        return &m_screenFbo.renderTexture;
    }

    void OpenglRenderModule::onWindowResize(Event& event)
    {

        RenderModule::onWindowResize(event);

        const auto& msg = static_cast<const WindowResizeEvent&>(event);

        if (m_scale == true)
        {
            return;
        }

        const auto screenWidth = msg.width;
        const auto screenHeight = msg.height;

        glViewport(0, 0, screenWidth, screenHeight);

        // resize render texture
        {
            m_screenFbo.renderTexture.texture.width = screenWidth;
            m_screenFbo.renderTexture.texture.height = screenHeight;
            m_screenFbo.renderTexture.depth.width = screenWidth;
            m_screenFbo.renderTexture.depth.height = screenHeight;

            GLint prevFbo;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFbo);
            {
                glBindFramebuffer(GL_FRAMEBUFFER, m_screenFbo.renderTexture.framebuffer);

                glBindTexture(GL_TEXTURE_2D, m_screenFbo.renderTexture.texture.id);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
                glBindTexture(GL_TEXTURE_2D, 0);

                glBindRenderbuffer(GL_RENDERBUFFER, m_screenFbo.renderTexture.depth.id);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
                glBindRenderbuffer(GL_RENDERBUFFER, 0);

                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
            glBindFramebuffer(GL_FRAMEBUFFER, prevFbo);

        }

    }

    void OpenglRenderModule::clearBackground(Color color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenglRenderModule::beginMode2D()
    {
        glDisable(GL_DEPTH_TEST);
    }

    void OpenglRenderModule::endMode2D()
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

    void OpenglRenderModule::beginMode3D()
    {
        glEnable(GL_DEPTH_TEST);
    }

    void OpenglRenderModule::endMode3D()
    {

    }

    void OpenglRenderModule::beginTextureMode()
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_screenFbo.renderTexture.texture.id);
    }

    void OpenglRenderModule::beginTextureMode(texture::RenderTexture2D& target)
    {
        glBindTexture(GL_TEXTURE_2D, target.texture.id);
    }

    void OpenglRenderModule::endTextureMode(void)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void OpenglRenderModule::beginShaderMode(aiko::Shader* shader)
    {
        glDisable(GL_CULL_FACE);
        shader->use();
    }

    void OpenglRenderModule::endShaderMode(void)
    {
        glUseProgram(0);
    }

    void OpenglRenderModule::beginBlendMode(BlendMode mode)
    {

    }

    void OpenglRenderModule::endBlendMode(void)
    {

    }

}
#endif