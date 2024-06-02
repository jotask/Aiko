#include "modules/render_module.h"


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
#include "constants.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

namespace aiko
{

    void RenderModule::initScreenFbo()
    {

        uint& quadVAO = m_screenFbo.vao;
        uint& quadVBO = m_screenFbo.vbo;

        constexpr const float quadVertices[] =
        {
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        texture::RenderTexture2D& target = m_screenFbo.renderTexture;

        auto size = m_displayModule->getCurrentDisplay().getDisplaySize();
        target.texture.width = size.x;
        target.texture.height = size.y;
        target.depth.width = size.x;
        target.depth.height = size.y;

        // Create and bind the framebuffer. This is done exactly the same as it's done for everything else in OpenGL.
        glGenFramebuffers(1, &target.framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, target.framebuffer); // The bind location form framebuffers is simply named GL_FRAMEBUFFER.

        // The framebuffer actually consists of a few smaller objects.
        // These are primarily textures and renderbuffers, each which have different uses.
        glGenTextures(1, &target.texture.id);
        glBindTexture(GL_TEXTURE_2D, target.texture.id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, target.texture.width, target.texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target.texture.id, 0);

        // clamp to border, do not allow texture wrapping
        // otherwise, pixels that go off the top of the screen will wrap to the bottom
        // and pixels that go too far right will show up on the left
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        // When we read from the texture, it will be 1 to 1 with the screen, so we shouldnt have any filtering.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenRenderbuffers(1, &target.depth.id);
        glBindRenderbuffer(GL_RENDERBUFFER, target.depth.id);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, target.depth.width, target.depth.height); // use a single renderbuffer object for both a depth AND stencil buffer.
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, target.depth.id); // now actually attach it
        // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            Log::error( "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" );
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
        unsigned char* data = stbi_load(global::getAssetPath("raysan.png").c_str(), &texture.width, &texture.height, &texture.channels, 0);
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
            Log::error("Failed to load texture");
        }
        stbi_image_free(data);

        return texture;
    }

    texture::PboTexture RenderModule::createPboTexture(uint16_t width, uint16_t height)
    {
        texture::PboTexture pbo;

        pbo.texture.width  = width;
        pbo.texture.height = height;
        pbo.texture.format = GL_RGBA;

        const auto channel_count = 4;
        pbo.data_size = width * height * channel_count;

        // Create texture
        {
            // init 2 texture objects
            glGenTextures(1, &pbo.texture.id);
            glBindTexture(GL_TEXTURE_2D, pbo.texture.id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        glGenBuffers(1, &pbo.pbo);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo.pbo);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, pbo.data_size, nullptr, GL_STREAM_DRAW);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

        return pbo;

    }

    void RenderModule::updatePboTexture(texture::PboTexture pbo, std::vector<Color>& pixels)
    {

        assert(pbo.texture.width * pbo.texture.height == pixels.size(), "Oh dear this doesn't work at all!");

        glBindTexture(GL_TEXTURE_2D, pbo.texture.id);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo.pbo);

        glBufferData(GL_PIXEL_UNPACK_BUFFER, pbo.data_size, nullptr, GL_STREAM_DRAW);

        GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
        if (ptr)
        {

            static auto updatePixels = [](GLubyte* dst, uint16_t width, uint16_t height)
            {
                    static int color = 0;
                    if (!dst) return;
                    int* ptr = (int*)dst;

                    // copy 4 bytes at once
                    for (int i = 0; i < height; ++i)
                    {
                        for (int j = 0; j < width; ++j)
                        {
                            *ptr = color;
                            ++ptr;
                        }
                        color += 257;
                    }
                    ++color;
            };

            for (auto c : pixels)
            {
               *ptr = c.r * 255;
               ++ptr;
               *ptr = c.g * 255;
               ++ptr;
               *ptr = c.b * 255;
               ++ptr;
               *ptr = c.a * 255;
               ++ptr;
            }

            // update data directly on the mapped buffer
            // updatePixels(ptr, pbo.texture.width, pbo.texture.height);
            glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);  // release pointer to mapping buffer
        }
        else
        {
            aiko::Log::error("Couldn't get PBO data");
        }

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, pbo.texture.width, pbo.texture.height, pbo.texture.format, GL_UNSIGNED_BYTE, nullptr);

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

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

