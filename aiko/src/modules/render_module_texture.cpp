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

    
    texture::RenderTexture2D RenderModule::createRenderTexture()
    {

        texture::RenderTexture2D target;

        auto size = m_displayModule->getCurrentDisplay().getDisplaySize();
        target.width = size.x;
        target.height = size.y;

        // Create and bind the framebuffer. This is done exactly the same as it's done for everything else in OpenGL.
        GLuint frameBuffer;
        glGenFramebuffers(1, &frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer); // The bind location form framebuffers is simply named GL_FRAMEBUFFER.

        // The framebuffer actually consists of a few smaller objects.
        // These are primarily textures and renderbuffers, each which have different uses.
        glGenTextures(1, &target.texture);
        glBindTexture(GL_TEXTURE_2D, target.texture);
        // Here we're going to create a texture that matches the size of our viewport. (this also gets resized in the viewport resizing code)
        // Instead of passing in data for the texture, we pass in null, which leaves the texture empty.
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, target.width, target.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        // clamp to border, do not allow texture wrapping
        // otherwise, pixels that go off the top of the screen will wrap to the bottom
        // and pixels that go too far right will show up on the left
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        // When we read from the texture, it will be 1 to 1 with the screen, so we shouldnt have any filtering.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);

        // We also need to create a render buffer.
        // A render buffer is similar to a texture, but with less features.
        // When rendering to a texture, we can later use that texture in another draw call, and draw it onto a surface.
        // A render buffer can't do that. The pixel data is temporary, and gets thrown away by the gpu when it's done.

        // We're going to create one here for the depth stencil buffer.
        // This isn't actually required for this example, because we only render one object, but if we were rendering with depth, this would be very important.
        glGenRenderbuffers(1, &target.depth);
        glBindRenderbuffer(GL_RENDERBUFFER, target.depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, target.width, target.height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        // Finally, we attach both the texture and render buffer to our frame buffer.
        // The texture is attached as a "color attachment". This is where our fragment shader outputs.
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target.texture, 0);
        // The render buffer is attached to the depth stencil attachment.
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, target.depth);

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
            Log::error("Failed to load texture");
        }
        stbi_image_free(data);

        return texture;
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

