#include "opengl_3d_renderer.h"

#include "core/libs.h"

#include "models/camera.h"

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
                uniform vec4 u_color;
                uniform sampler2D u_texture;
                void main()
                {
                    vec4 final = texture(u_texture, TexCoord) * u_color;
                    if(final.a < 0.001)
                    {
                        discard;
                    }
                    FragColor = final;
                }
            )";

            root.shader = getRenderModule()->createShader();
            root.shader->loadFromSource(vertexShaderSource, fragmentShaderSource);

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

            #define GL2D_OPNEGL_SHADER_VERSION "#version 330"
            #define GL2D_OPNEGL_SHADER_PRECISION "precision highp float;"

            static const char* vertexShaderSource =
                GL2D_OPNEGL_SHADER_VERSION "\n"
                GL2D_OPNEGL_SHADER_PRECISION "\n"
                "layout(location = 0) in vec2 quad_positions;\n"
                "layout(location = 1) in vec4 quad_colors;\n"
                "layout(location = 2) in vec2 texturePositions;\n"
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

            static const char* fragmentShaderSource =
                GL2D_OPNEGL_SHADER_VERSION "\n"
                GL2D_OPNEGL_SHADER_PRECISION "\n"
                "out vec4 color;\n"
                "in vec4 v_color;\n"
                "in vec2 v_texture;\n"
                "uniform sampler2D u_sampler;\n"
                "void main()\n"
                "{\n"
                "    vec4 texColor = texture(u_sampler, v_texture);\n"
                "    vec4 finalColor = v_color * texColor;\n"
                "    if (finalColor.a < 0.1) // Adjust threshold for transparency\n"
                "    {\n"
                // "        discard;\n"
                "    }\n"
                "    color = finalColor;\n"
                "}\n";

            objs.shader = getRenderModule()->createShader();
            objs.shader->loadFromSource(vertexShaderSource, fragmentShaderSource);

        }

        const size_t quadCount = 1'000;

        objs.spritePositions.reserve(quadCount * 6);
        objs.spriteColors.reserve(quadCount * 6);
        objs.texturePositions.reserve(quadCount * 6);
        objs.spriteTextures.reserve(quadCount);

    }

    void Opengl3DRenderer::beginFrame()
    {
    }

    void Opengl3DRenderer::endFrame()
    {

        // Use framebuffer
        // glBindFramebuffer(GL_FRAMEBUFFER, getRenderModule()->getRenderTexture()->framebuffer);
        // glViewport(0, 0, getRenderModule()->getRenderTexture()->texture.width, getRenderModule()->getRenderTexture()->texture.height); // Set viewport to match texture size

        // Enable blending for transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //for (auto& v : objs)
        if( objs.spriteTextures.empty() == false)
        {
            glBindVertexArray(objs.vao);
            objs.shader->use();
            objs.shader->setInt("u_sampler", 0);

            {
                std::vector<float> sp;
                for (auto& tmp : objs.spritePositions)
                {
                    sp.push_back(tmp.x);
                    sp.push_back(tmp.y);
                }
                glBindBuffer(GL_ARRAY_BUFFER, objs.buffers[Renderer2DBufferType::QuadPositions]);
                glBufferData(GL_ARRAY_BUFFER, sp.size() * sizeof(float), sp.data(), GL_STREAM_DRAW);
            }

            {
                std::vector<float> sc;
                for (auto& tmp : objs.spriteColors)
                {
                    sc.push_back(tmp.x);
                    sc.push_back(tmp.y);
                    sc.push_back(tmp.z);
                    sc.push_back(tmp.w);
                }
                glBindBuffer(GL_ARRAY_BUFFER, objs.buffers[Renderer2DBufferType::QuadColors]);
                glBufferData(GL_ARRAY_BUFFER, sc.size() * sizeof(float), sc.data(), GL_STREAM_DRAW);
            }

            {

                std::vector<float> tp;
                for (auto& tmp : objs.texturePositions)
                {
                    tp.push_back(tmp.x);
                    tp.push_back(tmp.y);
                }
                glBindBuffer(GL_ARRAY_BUFFER, objs.buffers[Renderer2DBufferType::TexturePositions]);
                glBufferData(GL_ARRAY_BUFFER, tp.size() * sizeof(float), tp.data(), GL_STREAM_DRAW);
            }

            //Instance render the textures
            {
                const int size = objs.spriteTextures.size();
                int pos = 0;
                unsigned int id = objs.spriteTextures[0].id;

                glUseProgram(objs.spriteTextures[0].id);

                for (int i = 1; i < size; i++)
                {
                    if (objs.spriteTextures[i].id != id)
                    {
                        glDrawArrays(GL_TRIANGLES, pos * 6, 6 * (i - pos));

                        pos = i;
                        id = objs.spriteTextures[i].id;
                        glUseProgram(objs.spriteTextures[i].id);
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

    void Opengl3DRenderer::dispose()
    {
    }

    void Opengl3DRenderer::drawRectangle(Camera* cam, vec2 pos, vec2 size, Color color)
    {
        vec2 newOrigin;
        drawAbsRotation(cam, pos, size, objs.defaultTexture, color, newOrigin, 0);
    }

    void Opengl3DRenderer::drawAbsRotation(Camera* cam, vec2 pos, vec2 size, const texture::Texture texture, const Color color, const vec2 origin, const float rotationDegrees, const vec4 textureCoords)
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

        objs.spritePositions.push_back(vec2{ v1.x, v1.y });
        objs.spritePositions.push_back(vec2{ v2.x, v2.y });
        objs.spritePositions.push_back(vec2{ v4.x, v4.y });

        objs.spritePositions.push_back(vec2{ v2.x, v2.y });
        objs.spritePositions.push_back(vec2{ v3.x, v3.y });
        objs.spritePositions.push_back(vec2{ v4.x, v4.y });

        objs.spriteColors.push_back(color.r);
        objs.spriteColors.push_back(color.g);
        objs.spriteColors.push_back(color.b);
        objs.spriteColors.push_back(color.g);
        objs.spriteColors.push_back(color.b);
        objs.spriteColors.push_back(color.a);

        objs.texturePositions.push_back(vec2{ textureCoords.x, textureCoords.y }); //1
        objs.texturePositions.push_back(vec2{ textureCoords.x, textureCoords.w }); //2
        objs.texturePositions.push_back(vec2{ textureCoords.z, textureCoords.y }); //4
        objs.texturePositions.push_back(vec2{ textureCoords.x, textureCoords.w }); //2
        objs.texturePositions.push_back(vec2{ textureCoords.z, textureCoords.w }); //3
        objs.texturePositions.push_back(vec2{ textureCoords.z, textureCoords.y }); //4

        objs.spriteTextures.push_back(textureCopy);

    }

    vec2 Opengl3DRenderer::positionToScreenCoords(vec2 pos, vec2 screenSize)
    {
        return
        {
            (pos.x / screenSize.x) * 2 - 1,
            -((-pos.y / screenSize.y) * 2 - 1)

        };
    }

    vec2 Opengl3DRenderer::rotateAroundPoint(vec2 vec, vec2 point, float degrees)
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

    vec2 Opengl3DRenderer::scaleAroundPoint(vec2 vec, vec2 point, float scale)
    {
        vec = (vec - point) * scale + point;
        return vec;
    }

}