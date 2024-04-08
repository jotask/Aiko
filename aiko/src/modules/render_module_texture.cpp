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

    texture::ScreenTexture2D RenderModule::createScreenTexture()
    {

        texture::ScreenTexture2D screen;

        // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        static float quadVertices[] = {
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };

        // screen quad VAO
        unsigned int quadVAO, quadVBO;
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
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

        // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
        {
            GLuint FramebufferName = 0;
            glGenFramebuffers(1, &FramebufferName);
            glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
        }

        // The texture we're going to render to
        GLuint renderedTexture;
        {

            glGenTextures(1, &renderedTexture);

            // "Bind" the newly created texture : all future texture functions will modify this texture
            glBindTexture(GL_TEXTURE_2D, renderedTexture);

            // Give an empty image to OpenGL ( the last "0" )
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 768, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

            // Poor filtering. Needed !
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }

        // The depth buffer
        {
            GLuint depthrenderbuffer;
            glGenRenderbuffers(1, &depthrenderbuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
        }

        {
            // Set "renderedTexture" as our colour attachement #0
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

            // Set the list of draw buffers.
            GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
            glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
        }

        // Always check that our framebuffer is ok
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "ERROR::MODULE::RENDERTEXTURE::COMPILATION_FAILED\n" << std::endl;
        }

        return texture::RenderTexture2D
        {

        };
    }

    texture::Texture RenderModule::createTexture()
    {

    }

    void RenderModule::drawTextureEx(texture::Texture texture, vec2 position, float rotation, float scale, Color tint)
    {
        int a = 10;
    }

}

