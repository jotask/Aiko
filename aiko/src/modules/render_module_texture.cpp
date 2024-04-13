#include "modules/render_module.h"

#include <string>
#include <fstream>

#include <core/libs.h>

#include "modules/module_connector.h"
#include "modules/display_module.h"
#include "models/camera.h"
#include "types/textures.h"
#include "events/events.hpp"
#include "types/render_types.h"
#include "models/mesh.h"
#include "models/shader.h"
#include "components/transform_component.h"
#include "modules/render_primitives.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

namespace aiko
{

    texture::ScreenTexture2D RenderModule::createScreenTexture()
    {

        texture::ScreenTexture2D screen;

        static float quadVertices[] =
        { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };

        glGenVertexArrays(1, &screen.vao);
        glGenBuffers(1, &screen.vbo);
        glBindVertexArray(screen.vao);
        glBindBuffer(GL_ARRAY_BUFFER, screen.vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        screen.renderTexture = createRenderTexture();

        return screen;
    }

    texture::RenderTexture2D RenderModule::createRenderTexture()
    {

        texture::RenderTexture2D target;

        // TMP
        auto size = m_displayModule->getCurrentDisplay().getDisplaySize();

        // framebuffer configuration
        // -------------------------
        glGenFramebuffers(1, &target.framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, target.framebuffer);

        // create a color attachment texture
        unsigned int textureColorbuffer;
        glGenTextures(1, &textureColorbuffer);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

        // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
        unsigned int rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y); // use a single renderbuffer object for both a depth AND stencil buffer.
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
        // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        target.texture = textureColorbuffer;
        target.width = size.x;
        target.height = size.y;
        target.depth = rbo;

        return target;

    }

    void RenderModule::renderToTargetTexture()
    {
        // TODO
    }

    texture::Texture RenderModule::createTexture()
    {
        // load and create a texture 
        // -------------------------
        texture::Texture texture;
        // texture 1
        // ---------
        glGenTextures(1, &texture.id);
        glBindTexture(GL_TEXTURE_2D, texture.id);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load image, create texture and generate mipmaps
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
        // TODO
        unsigned char* data = stbi_load("C:/Users/j.iznardo/Desktop/awesomeface.png", &texture.width, &texture.height, &texture.channels, 0);
        if (data)
        {

            if (texture.channels == 3)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.width, texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
            }
            else if (texture.channels == 4)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
            }

        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);

        return texture;
    }

    void RenderModule::drawToScreenTexture()
    {
        auto pos = vec3(0.0f);
        auto size = vec2(2.0f);

        float halfWidth = size.x / 2.0f;
        float halfHeight = size.y / 2.0f;

        std::vector<GLfloat> rectangleVertices =
        {
            pos.x + halfWidth, pos.y + halfHeight, pos.z,
            pos.x - halfWidth, pos.y + halfHeight, pos.z,
            pos.x - halfWidth, pos.y - halfHeight, pos.z,
            pos.x - halfWidth, pos.y - halfHeight, pos.z,
            pos.x + halfWidth, pos.y - halfHeight, pos.z,
            pos.x + halfWidth, pos.y + halfHeight, pos.z,
        };

        // Create and bind VAO and VBO
        GLuint VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // bindData(rectangleVertices);
        {
            std::vector<Primitives::Data::Vertex> vertices;
            for (size_t idx = 0; idx < rectangleVertices.size(); idx += 3)
            {
                float x = rectangleVertices[idx + 0];
                float y = rectangleVertices[idx + 1];
                float z = rectangleVertices[idx + 2];
                vec3 position = { x, y, z };
                vec3 normal = { 0, 0, 0 };
                vec2 uv = { 0, 0 };
                vertices.push_back({ position, normal, uv });
            }

            Primitives::Data data = { vertices };
            Primitives::calculateNormals(data);
            Primitives::calculateUvs(data);

            std::vector<GLfloat> flattenedVertices;
            for (const auto& vertex : data.vertices)
            {
                flattenedVertices.push_back(vertex.position.x);
                flattenedVertices.push_back(vertex.position.y);
                flattenedVertices.push_back(vertex.position.z);

                flattenedVertices.push_back(vertex.normal.x);
                flattenedVertices.push_back(vertex.normal.y);
                flattenedVertices.push_back(vertex.normal.z);

                flattenedVertices.push_back(vertex.texCoords.x);
                flattenedVertices.push_back(vertex.texCoords.y);
            }
            glBufferData(GL_ARRAY_BUFFER, flattenedVertices.size() * sizeof(GLfloat), flattenedVertices.data(), GL_STATIC_DRAW);
        }

        // bindShaderAttributes();
        {
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glEnableVertexAttribArray(2);
        }

        // setUniforms(color);

        // Draw rectangle
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Unbind VAO and VBO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Delete VAO and VBO
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);

        glEnable(GL_CULL_FACE);
    }

    void RenderModule::drawTextureEx(texture::Texture texture, vec2 position, float rotation, float scale, Color tint)
    {
        int a = 10;
    }

    void RenderModule::drawRenderTextureEx(texture::RenderTexture2D texture, vec2 position, float rotation, float scale, Color tint)
    {
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, texture.texture);
        // glUniform1i(m_textureUniformID, 0);
    }

}

