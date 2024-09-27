#include "opengl_2d_renderer.h"

#include "core/libs.h"

#include "components/transform_component.h"

#include "shared/math.h"
#include "models/camera.h"
#include "constants.h"

namespace aiko
{

    constexpr float s_rotation = 0.0f;
    constexpr float s_zoom = 4.0f;

    Opengl2DRenderer::Opengl2DRenderer(RenderModule* renderer)
        : RenderContext2D(renderer)
    {
    }

    void Opengl2DRenderer::init()
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

            glGenVertexArrays(1, &root.vao);
            glGenBuffers(1, &root.vbo);
            glBindVertexArray(root.vao);

            glBindBuffer(GL_ARRAY_BUFFER, root.vbo);
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
            root.shader = getRenderModule()->createShader();
            root.shader->load(
                aiko::global::getAssetPath("shaders/aiko_2d.vs").c_str(),
                aiko::global::getAssetPath("shaders/aiko_2d.fs").c_str()
            );
        }
        // Overlay
        {

            glGenVertexArrays(1, &objs.vao);
            glBindVertexArray(objs.vao);

            // Create Buffers
            glGenBuffers(Renderer2DBufferType::BufferSize, objs.buffers);

            glBindBuffer(GL_ARRAY_BUFFER, objs.buffers[Renderer2DBufferType::QuadPositions]);
            glEnableVertexAttribArray(Renderer2DBufferType::QuadPositions);
            glVertexAttribPointer(Renderer2DBufferType::QuadPositions, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

            glBindBuffer(GL_ARRAY_BUFFER, objs.buffers[Renderer2DBufferType::QuadColors]);
            glEnableVertexAttribArray(Renderer2DBufferType::QuadColors);
            glVertexAttribPointer(Renderer2DBufferType::QuadColors, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

            glBindBuffer(GL_ARRAY_BUFFER, objs.buffers[Renderer2DBufferType::TexturePositions]);
            glEnableVertexAttribArray(Renderer2DBufferType::TexturePositions);
            glVertexAttribPointer(Renderer2DBufferType::TexturePositions, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
        {
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
            objs.defaultTexture = createFromBuffer((char*)buff, 1, 1);
        }
        {
            objs.shader = getRenderModule()->createShader();
            objs.shader->load(
                aiko::global::getAssetPath("shaders/aiko_2d_batch.vs").c_str(),
                aiko::global::getAssetPath("shaders/aiko_2d_batch.fs").c_str()
            );
        }

        const size_t quadCount = 1'000;

        objs.spritePositions.reserve(quadCount * 6);
        objs.spriteColors.reserve(quadCount * 6);
        objs.texturePositions.reserve(quadCount * 6);
        objs.spriteTextures.reserve(quadCount);

    }

    void Opengl2DRenderer::beginFrame()
    {


        {
            // Define the camera's position, target, and up vector
            vec3 cameraPos = vec3(0.0f, 0.0f, 0.5f);  // Camera position
            vec3 cameraTarget = vec3(0.0f, 0.0f, 0.0f);  // Target the camera is looking at
            vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);  // Up direction
            objs.view = math::lookAt(cameraPos, cameraTarget, cameraUp);
        }

        {
            float fov = 120.0f;   // Field of view in radians
            float width = 800.0f;
            float height = 600.0f;
            float aspectRatio = width / height; // Screen aspect ratio (width/height)
            float nearPlane = 0.1f;  // Near clipping plane
            float farPlane = 100.0f;  // Far clipping plane
            objs.projection = math::perspective(fov, width, height, nearPlane, farPlane);
        }

    }

    void Opengl2DRenderer::endFrame()
    {

        // Use framebuffer
        // glBindFramebuffer(GL_FRAMEBUFFER, getRenderModule()->getRenderTexture()->framebuffer);
        // glViewport(0, 0, getRenderModule()->getRenderTexture()->texture.width, getRenderModule()->getRenderTexture()->texture.height); // Set viewport to match texture size

        // Enable blending for transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //for (auto& v : objs)
        if (objs.spriteTextures.empty() == false)
        {

            glBindVertexArray(objs.vao);
            objs.shader->use();
            objs.shader->setInt("u_sampler", 0);
            objs.shader->setMat4("projection", objs.projection);
            objs.shader->setMat4("view", objs.view);
            Transform trans;
            trans.position = {0};
            trans.rotation = { 1.0f };
            trans.scale = { 1 };
            objs.shader->setMat4("model", trans.getMatrix());

            glBindBuffer(GL_ARRAY_BUFFER, objs.buffers[Renderer2DBufferType::QuadPositions]);
            glBufferData(GL_ARRAY_BUFFER, objs.spritePositions.size() * sizeof(vec2), objs.spritePositions.data(), GL_STREAM_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, objs.buffers[Renderer2DBufferType::QuadColors]);
            glBufferData(GL_ARRAY_BUFFER, objs.spriteColors.size() * sizeof(Color), objs.spriteColors.data(), GL_STREAM_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, objs.buffers[Renderer2DBufferType::TexturePositions]);
            glBufferData(GL_ARRAY_BUFFER, objs.texturePositions.size() * sizeof(vec2), objs.texturePositions.data(), GL_STREAM_DRAW);

            //Instance render the textures
            {
                const int size = objs.spriteTextures.size();
                int pos = 0;
                unsigned int id = objs.spriteTextures[0].id;

                glActiveTexture(GL_TEXTURE0 + 0);
                glBindTexture(GL_TEXTURE_2D, objs.spriteTextures[0].id);

                for (int i = 1; i < size; i++)
                {
                    if (objs.spriteTextures[i].id != id)
                    {
                        glDrawArrays(GL_TRIANGLES, pos * 6, 6 * (i - pos));

                        pos = i;
                        id = objs.spriteTextures[i].id;
                        glActiveTexture(GL_TEXTURE0 + 0);
                        glBindTexture(GL_TEXTURE_2D, objs.spriteTextures[i].id);
                    }

                }

                glDrawArrays(GL_TRIANGLES, pos * 6, 6 * (size - pos));

                glBindVertexArray(0);
            }

            objs.shader->unuse();

            objs.spritePositions.clear();
            objs.spriteColors.clear();
            objs.texturePositions.clear();
            objs.spriteTextures.clear();

        }

        // Bind shader (assuming you have a shader program already created and compiled)
        root.shader->use();

        // Bind the VAO
        glBindVertexArray(root.vao);

        // Bind the texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, getRenderModule()->getRenderTexture()->texture.id);
        root.shader->setInt("u_texture", 0);
        root.shader->setVec4("u_color", { 1.0f, 1.0f, 1.0f, 1.0f });

        // Draw the quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Unbind everything
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        root.shader->unuse();

        // Unbind framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

    void Opengl2DRenderer::dispose()
    {
    }

    void Opengl2DRenderer::drawRectangle(Camera* cam, vec2 pos, vec2 size, Color color)
    {
        vec2 newOrigin = pos;
        Color colors[4] = {color, color, color, color};
        drawAbsRotation(cam, pos, size, objs.defaultTexture, colors, newOrigin, 0);
    }

    void Opengl2DRenderer::drawRectangle(Camera* cam, vec2 pos, vec2 size, Color colors[4])
    {
        vec2 newOrigin;
        drawAbsRotation(cam, pos, size, objs.defaultTexture, colors, newOrigin, 0);
    }

    void Opengl2DRenderer::drawAbsRotation(Camera* cam, vec2 pos, vec2 size, const texture::Texture texture, const Color colors[4], const vec2 origin, const float rotationDegrees, const vec4 textureCoords)
    {
        texture::Texture textureCopy = texture;

        if (textureCopy.id == 0)
        {
            Log::error("Invalid texture");
            textureCopy = objs.defaultTexture;
        }

        //We need to flip texture_transforms.y
        const float transformsY = pos.y * -1;

        vec2 v1 = { pos.x,         transformsY };
        vec2 v2 = { pos.x,         transformsY - size.x };
        vec2 v3 = { pos.x + size.y, transformsY - size.x };
        vec2 v4 = { pos.x + size.y, transformsY };

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

        const auto screenSize = getRenderModule()->getDisplaySize();

        //Apply camera rotation
        if (s_rotation != 0)
        {
            vec2 cameraCenter = { screenSize.x / 2.0f, screenSize.y / 2.0f };
            v1 = rotateAroundPoint(v1, cameraCenter, s_rotation);
            v2 = rotateAroundPoint(v2, cameraCenter, s_rotation);
            v3 = rotateAroundPoint(v3, cameraCenter, s_rotation);
            v4 = rotateAroundPoint(v4, cameraCenter, s_rotation);
        }

        //Apply camera zoom
        if(s_zoom != 1)
        {
            vec2 cameraCenter = { cam->position.x , cam->position.y };
            v1 = scaleAroundPoint(v1, cameraCenter, s_zoom);
            v2 = scaleAroundPoint(v2, cameraCenter, s_zoom);
            v3 = scaleAroundPoint(v3, cameraCenter, s_zoom);
            v4 = scaleAroundPoint(v4, cameraCenter, s_zoom);
        }

        v1 = positionToScreenCoords(v1, screenSize);
        v2 = positionToScreenCoords(v2, screenSize);
        v3 = positionToScreenCoords(v3, screenSize);
        v4 = positionToScreenCoords(v4, screenSize);

        objs.spritePositions.push_back(vec2{ v1.x, v1.y });
        objs.spritePositions.push_back(vec2{ v2.x, v2.y });
        objs.spritePositions.push_back(vec2{ v4.x, v4.y });

        objs.spritePositions.push_back(vec2{ v2.x, v2.y });
        objs.spritePositions.push_back(vec2{ v3.x, v3.y });
        objs.spritePositions.push_back(vec2{ v4.x, v4.y });

        objs.spriteColors.push_back(colors[0]); // v1
        objs.spriteColors.push_back(colors[1]); // v2
        objs.spriteColors.push_back(colors[3]); // v4

        objs.spriteColors.push_back(colors[1]); // v2
        objs.spriteColors.push_back(colors[2]); // v3
        objs.spriteColors.push_back(colors[3]); // v4

        objs.texturePositions.push_back(vec2{ textureCoords.x, textureCoords.y }); //1
        objs.texturePositions.push_back(vec2{ textureCoords.x, textureCoords.w }); //2
        objs.texturePositions.push_back(vec2{ textureCoords.z, textureCoords.y }); //4
        objs.texturePositions.push_back(vec2{ textureCoords.x, textureCoords.w }); //2
        objs.texturePositions.push_back(vec2{ textureCoords.z, textureCoords.w }); //3
        objs.texturePositions.push_back(vec2{ textureCoords.z, textureCoords.y }); //4

        objs.spriteTextures.push_back(textureCopy);

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