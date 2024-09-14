#include "opengl_3d_renderer.h"

#include "core/libs.h"

namespace aiko
{

    Opengl3DRenderer::Opengl3DRenderer(RenderModule* renderer)
        : RenderContext3D(renderer)
    {
    }

    void Opengl3DRenderer::init()
    {
        // Passthrought
        {
            static float quadVertices[] =
            {
                // Positions     // Texture Coords
                -1.0f,  1.0f,    0.0f, 1.0f,
                -1.0f, -1.0f,    0.0f, 0.0f,
                 1.0f, -1.0f,    1.0f, 0.0f,
                             
                -1.0f,  1.0f,    0.0f, 1.0f,
                 1.0f, -1.0f,    1.0f, 0.0f,
                 1.0f,  1.0f,    1.0f, 1.0f
            };

            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);
            glBindVertexArray(vao);

            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

            // Position attribute
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            // Texture coordinate attribute
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
            glEnableVertexAttribArray(1);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
        {
            // Shader sources
            const char* vertexShaderSource = R"(
                #version 330 core
                layout(location = 0) in vec2 aPosition;
                layout(location = 1) in vec2 aTexCoord;
                out vec2 TexCoord;
                void main()
                {
                    gl_Position = vec4(aPosition, 0.0, 1.0);
                    TexCoord = aTexCoord;
                }
            )";

            const char* fragmentShaderSource = R"(
                #version 330 core
                in vec2 TexCoord;
                out vec4 FragColor;
                uniform sampler2D u_texture;
                void main()
                {
                    // FragColor = texture(u_texture, TexCoord);
                    FragColor = vec4(0.23f, 1.0f, 0.75f, 0.5f);
                }
            )";

            m_shader = getRenderModule()->createShader();
            m_shader->loadFromSource(vertexShaderSource, fragmentShaderSource);

        }
        // Overlay
        {
            static float overlayVertices[] =
            {
                // Positions     // Texture Coords
                -0.5f,  0.5f,    0.0f, 1.0f,
                -0.5f, -0.5f,    0.0f, 0.0f,
                 0.5f, -0.5f,    1.0f, 0.0f,

                -0.5f,  0.5f,    0.0f, 1.0f,
                 0.5f, -0.5f,    1.0f, 0.0f,
                 0.5f,  0.5f,    1.0f, 1.0f
            };

            glGenVertexArrays(1, &vao2);
            glGenBuffers(1, &vbo2);
            glBindVertexArray(vao2);

            glBindBuffer(GL_ARRAY_BUFFER, vbo2);
            glBufferData(GL_ARRAY_BUFFER, sizeof(overlayVertices), overlayVertices, GL_STATIC_DRAW);

            // Position attribute
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            // Texture coordinate attribute
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
            glEnableVertexAttribArray(1);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
        {
            // Shader sources
            const char* vertexShaderSource = R"(
                #version 330 core
                layout(location = 0) in vec2 aPosition;
                layout(location = 1) in vec2 aTexCoord;
                out vec2 TexCoord;
                void main()
                {
                    gl_Position = vec4(aPosition, 0.0, 1.0);
                    TexCoord = aTexCoord;
                }
            )";

            const char* fragmentShaderSource = R"(
                #version 330 core
                in vec2 TexCoord;
                out vec4 FragColor;
                uniform vec4 finalColor;
                void main()
                {
                    FragColor = finalColor;
                }
            )";

            m_shader2 = getRenderModule()->createShader();
            m_shader2->loadFromSource(vertexShaderSource, fragmentShaderSource);

        }
    }

    void Opengl3DRenderer::beginFrame()
    {
    }

    void Opengl3DRenderer::endFrame()
    {

        // Use framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, getRenderModule()->getRenderTexture()->framebuffer);
        glViewport(0, 0, getRenderModule()->getRenderTexture()->texture.width, getRenderModule()->getRenderTexture()->texture.height); // Set viewport to match texture size

        // Enable blending for transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (true)
        {
            m_shader2->use();
            m_shader2->setVec4("finalColor", {1.0f, 0.0f, 1.0f, 1.0f});
            glBindVertexArray(vao2);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
            m_shader2->unuse();
        }

        // Bind shader (assuming you have a shader program already created and compiled)
        m_shader->use();

        // Bind the VAO
        glBindVertexArray(vao);

        // Bind the texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, getRenderModule()->getRenderTexture()->texture.id);
        m_shader->setInt("u_texture", 0);

        // Draw the quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Unbind everything
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        m_shader->unuse();

        // Unbind framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

    void Opengl3DRenderer::dispose()
    {
    }

}