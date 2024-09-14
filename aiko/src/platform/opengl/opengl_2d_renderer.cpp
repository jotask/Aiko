#include "opengl_2d_renderer.h"

#include "core/libs.h"
#include "modules/render/render_module.h"
#include "types/render_types.h"
#include "shared/math.h"
#include "models/camera.h"
#include "models/shader.h"

namespace aiko
{
    Opengl2DRenderer::Opengl2DRenderer(RenderModule* renderer)
        : RenderContext2D(renderer)
    {

    }

    void Opengl2DRenderer::init()
    {

        clearBatch();

        m_spritePositions.reserve(BATCH_SIZE * 6);
        m_spriteColors.reserve(BATCH_SIZE * 6);
        m_texturePositions.reserve(BATCH_SIZE * 6);
        m_spriteTextures.reserve(BATCH_SIZE);

        glGenVertexArrays(1, &m_fbo.vao);
        glBindVertexArray(m_fbo.vao);

        {

            auto size = getRenderModule()->getDisplaySize();
            m_fbo.renderTexture.texture.width = size.x;
            m_fbo.renderTexture.texture.height = size.y;
            m_fbo.renderTexture.depth.width = size.x;
            m_fbo.renderTexture.depth.height = size.y;

            // Create and bind the framebuffer. This is done exactly the same as it's done for everything else in OpenGL.
            glGenFramebuffers(1, &m_fbo.renderTexture.framebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, m_fbo.renderTexture.framebuffer); // The bind location form framebuffers is simply named GL_FRAMEBUFFER.

            // The framebuffer actually consists of a few smaller objects.
            // These are primarily textures and renderbuffers, each which have different uses.
            glGenTextures(1, &m_fbo.renderTexture.texture.id);
            glBindTexture(GL_TEXTURE_2D, m_fbo.renderTexture.texture.id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_fbo.renderTexture.texture.width, m_fbo.renderTexture.texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo.renderTexture.texture.id, 0);

            // clamp to border, do not allow texture wrapping
            // otherwise, pixels that go off the top of the screen will wrap to the bottom
            // and pixels that go too far right will show up on the left
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

            // When we read from the texture, it will be 1 to 1 with the screen, so we shouldnt have any filtering.
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glBindTexture(GL_TEXTURE_2D, 0);

            glGenRenderbuffers(1, &m_fbo.renderTexture.depth.id);
            glBindRenderbuffer(GL_RENDERBUFFER, m_fbo.renderTexture.depth.id);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_fbo.renderTexture.depth.width, m_fbo.renderTexture.depth.height); // use a single renderbuffer object for both a depth AND stencil buffer.
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fbo.renderTexture.depth.id); // now actually attach it
            // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                Log::error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

        }

        glGenBuffers(Renderer2DBufferType::BufferSize, buffers);

        glBindBuffer(GL_ARRAY_BUFFER, buffers[Renderer2DBufferType::QuadPositions]);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glBindBuffer(GL_ARRAY_BUFFER, buffers[Renderer2DBufferType::QuadColors]);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glBindBuffer(GL_ARRAY_BUFFER, buffers[Renderer2DBufferType::TexturePositions]);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glBindVertexArray(0);

        #define GL2D_OPNEGL_SHADER_VERSION "#version 330"
        #define GL2D_OPNEGL_SHADER_PRECISION "precision highp float;"

        static const char* defaultVertexShader =
            GL2D_OPNEGL_SHADER_VERSION "\n"
            GL2D_OPNEGL_SHADER_PRECISION "\n"
            "in vec2 quad_positions;\n"
            "in vec4 quad_colors;\n"
            "in vec2 texturePositions;\n"
            "out vec4 v_color;\n"
            "out vec2 v_texture;\n"
            "out vec2 v_positions;\n"
            "void main()\n"
            "{\n"
            "	gl_Position = vec4(quad_positions, 0, 1);\n"
            "	v_color = quad_colors;\n"
            "	v_texture = texturePositions;\n"
            "	v_positions = gl_Position.xy;\n"
            "}\n";

        static const char* defaultFragmentShader =
            GL2D_OPNEGL_SHADER_VERSION "\n"
            GL2D_OPNEGL_SHADER_PRECISION "\n"
            "out vec4 color;\n"
            "in vec4 v_color;\n"
            "in vec2 v_texture;\n"
            "uniform sampler2D u_sampler;\n"
            "void main()\n"
            "{\n"
            "    color = v_color * texture2D(u_sampler, v_texture);\n"
            "}\n";

        m_defaultShader = getRenderModule()->createShader();
        m_defaultShader->loadFromSource(defaultVertexShader, defaultFragmentShader);

        auto createFromBuffer = [](const char* image_data, const int width, const int height, bool pixelated = false, bool useMipMaps = true) -> texture::Texture
        {

            texture::Texture texture;
            texture.width = width;
            texture.height = height;

            glActiveTexture(GL_TEXTURE0);

            glGenTextures(1, &texture.id);
            glBindTexture(GL_TEXTURE_2D, texture.id);

            if (pixelated)
            {
                if (useMipMaps)
                {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                }
                else
                {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                }
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            }
            else
            {
                if (useMipMaps)
                {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                }
                else
                {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                }
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
            glGenerateMipmap(GL_TEXTURE_2D);

            return texture;

        };

        const unsigned char buff[] =
        {
            0xff,
            0xff,
            0xff,
            0xff
        };

        m_white1pxSquareTexture = createFromBuffer((char*)buff, 1, 1);

    }

    void Opengl2DRenderer::beginFrame()
    {

    }

    void Opengl2DRenderer::endFrame()
    {

        enableNecessaryGLFeatures();


        if (!m_fbo.vao)
        {
            Log::error("Renderer not initialized. It should be called after FBO is created");
            clearBatch();
            return;
        }

        if (m_spriteTextures.empty())
        {
            return;
        }

        auto windowSize = getRenderModule()->getDisplaySize();
        glViewport(0, 0, windowSize.x, windowSize.y);

        glBindVertexArray(m_fbo.vao);

        m_defaultShader->use();
        m_defaultShader->setInt("u_sampler", 0);

        glBindBuffer(GL_ARRAY_BUFFER, buffers[Renderer2DBufferType::QuadPositions]);
        glBufferData(GL_ARRAY_BUFFER, m_spritePositions.size() * sizeof(vec2), m_spritePositions.data(), GL_STREAM_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, buffers[Renderer2DBufferType::QuadColors]);
        glBufferData(GL_ARRAY_BUFFER, m_spriteColors.size() * sizeof(vec4), m_spriteColors.data(), GL_STREAM_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, buffers[Renderer2DBufferType::TexturePositions]);
        glBufferData(GL_ARRAY_BUFFER, m_texturePositions.size() * sizeof(vec2), m_texturePositions.data(), GL_STREAM_DRAW);

        //Instance render the textures
        {
            const int size = m_spriteTextures.size();
            int pos = 0;
            unsigned int id = m_spriteTextures[0].id;

            auto bindTexture = [](texture::Texture& text)
            {
                glActiveTexture(GL_TEXTURE0 + 0);
                glBindTexture(GL_TEXTURE_2D, text.id);
            };

            auto unbindTexture = [](texture::Texture& text)
            {
                glBindTexture(GL_TEXTURE_2D, 0);
            };

            bindTexture(m_spriteTextures[0]);

            for (int i = 1; i < size; i++)
            {
                if (m_spriteTextures[i].id != id)
                {
                    glDrawArrays(GL_TRIANGLES, pos * 6, 6 * (i - pos));
                    pos = i;
                    id = m_spriteTextures[i].id;
                    bindTexture(m_spriteTextures[i]);
                }
            }

            glDrawArrays(GL_TRIANGLES, pos * 6, 6 * (size - pos));

            glBindVertexArray(0);

            std::for_each(m_spriteTextures.begin(), m_spriteTextures.end(), [](texture::Texture& text) { glBindTexture(GL_TEXTURE_2D, 0); });

        }

        m_defaultShader->unuse();

        clearBatch();
        {
        }

    }

    void Opengl2DRenderer::dispose()
    {

    }

    void Opengl2DRenderer::drawRectangle(Camera* cam, vec2 pos, vec2 size, Color color)
    {
        vec2 newOrigin;
        drawAbsRotation(cam, pos, size, m_white1pxSquareTexture, color, newOrigin, 0);
    }

    void Opengl2DRenderer::drawAbsRotation(Camera* cam, vec2 pos, vec2 size, const texture::Texture texture, const Color color, const vec2 origin, const float rotationDegrees, const vec4 textureCoords)
    {

        texture::Texture textureCopy = texture;

        if (textureCopy.id == 0)
        {
            Log::error("Invalid texture");
            textureCopy = m_white1pxSquareTexture;
        }

        //We need to flip texture_transforms.y
        const float transformsY = pos.y * -1;

        vec2 v1 = { pos.x,         transformsY };
        vec2 v2 = { pos.x,         transformsY - size.x };
        vec2 v3 = { pos.x + pos.y, transformsY - size.x };
        vec2 v4 = { pos.x + pos.y, transformsY };

        //Apply rotations
        if (rotationDegrees != 0)
        {
            v1 = rotateAroundPoint(v1, origin, rotationDegrees);
            v2 = rotateAroundPoint(v2, origin, rotationDegrees);
            v3 = rotateAroundPoint(v3, origin, rotationDegrees);
            v4 = rotateAroundPoint(v4, origin, rotationDegrees);
        }

        //Apply camera transformations
        v1.x -= cam->position.x;
        v1.y += cam->position.y;
        v2.x -= cam->position.x;
        v2.y += cam->position.y;
        v3.x -= cam->position.x;
        v3.y += cam->position.y;
        v4.x -= cam->position.x;
        v4.y += cam->position.y;

        //Apply camera rotation
        /*
        if (cam->rotation != 0)
        {
            vec2 cameraCenter;

            cameraCenter.x = windowW / 2.0f;
            cameraCenter.y = windowH / 2.0f;

            v1 = rotateAroundPoint(v1, cameraCenter, currentCamera.rotation);
            v2 = rotateAroundPoint(v2, cameraCenter, currentCamera.rotation);
            v3 = rotateAroundPoint(v3, cameraCenter, currentCamera.rotation);
            v4 = rotateAroundPoint(v4, cameraCenter, currentCamera.rotation);
        }
        */

        //Apply camera zoom
        /*
        if(renderer->currentCamera.zoom != 1)
        {

            vec2 cameraCenter;
            cameraCenter.x = windowW / 2.0f;
            cameraCenter.y = -windowH / 2.0f;

            v1 = scaleAroundPoint(v1, cameraCenter, currentCamera.zoom);
            v2 = scaleAroundPoint(v2, cameraCenter, currentCamera.zoom);
            v3 = scaleAroundPoint(v3, cameraCenter, currentCamera.zoom);
            v4 = scaleAroundPoint(v4, cameraCenter, currentCamera.zoom);
        }
        */

        const auto screenSize = getRenderModule()->getDisplaySize();

        v1 = positionToScreenCoords(v1, screenSize);
        v2 = positionToScreenCoords(v2, screenSize);
        v3 = positionToScreenCoords(v3, screenSize);
        v4 = positionToScreenCoords(v4, screenSize);

        m_spritePositions.push_back(vec2{ v1.x, v1.y });
        m_spritePositions.push_back(vec2{ v2.x, v2.y });
        m_spritePositions.push_back(vec2{ v4.x, v4.y });

        m_spritePositions.push_back(vec2{ v2.x, v2.y });
        m_spritePositions.push_back(vec2{ v3.x, v3.y });
        m_spritePositions.push_back(vec2{ v4.x, v4.y });

        m_spriteColors.push_back(color.r);
        m_spriteColors.push_back(color.g);
        m_spriteColors.push_back(color.b);
        m_spriteColors.push_back(color.g);
        m_spriteColors.push_back(color.b);
        m_spriteColors.push_back(color.a);

        m_texturePositions.push_back(vec2{ textureCoords.x, textureCoords.y }); //1
        m_texturePositions.push_back(vec2{ textureCoords.x, textureCoords.w }); //2
        m_texturePositions.push_back(vec2{ textureCoords.z, textureCoords.y }); //4
        m_texturePositions.push_back(vec2{ textureCoords.x, textureCoords.w }); //2
        m_texturePositions.push_back(vec2{ textureCoords.z, textureCoords.w }); //3
        m_texturePositions.push_back(vec2{ textureCoords.z, textureCoords.y }); //4

        m_spriteTextures.push_back(textureCopy);

    }

    void Opengl2DRenderer::clearBatch()
    {
        m_spritePositions.clear();
        m_spriteColors.clear();
        m_texturePositions.clear();
        m_spriteTextures.clear();
    }

    void Opengl2DRenderer::enableNecessaryGLFeatures()
    {
        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    }

    vec2 Opengl2DRenderer::positionToScreenCoords(vec2 pos, vec2 screenSize)
    {
        return
        {
            (pos.x / screenSize.x) * 2 - 1,
            -((-pos.y / screenSize.y) * 2 - 1)

        };
    }

    vec2 Opengl2DRenderer::rotateAroundPoint(vec2 vec, vec2 point, float degrees)
    {
        point.y = -point.y;
        float a = math::radians(degrees);
        float s = sinf(a);
        float c = cosf(a);
        vec.x -= point.x;
        vec.y -= point.y;
        float newx = vec.x * c - vec.y * s;
        float newy = vec.x * s + vec.y * c;
        // translate point back:
        vec.x = newx + point.x;
        vec.y = newy + point.y;
        return vec;
    }

    vec2 Opengl2DRenderer::scaleAroundPoint(vec2 vec, vec2 point, float scale)
    {
        vec = (vec - point) * scale + point;
        return vec;
    }

}
