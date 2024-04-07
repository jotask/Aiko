#include "render_primitives.h"

#include "shared/math.h"
#include "shared/math_transform.h"
#include "systems/render_system.h"
#include "models/camera.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace aiko
{

    AikoPtr<Shader> Primitives::shader = nullptr;
    RenderSystem* Primitives::renderSystem = nullptr;

    unsigned int loadTexture(char const* path)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
        if (data == nullptr)
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
            return 0;
        }

        GLenum format = GL_RGBA;
        if (nrComponents == 1)
        {
            format = GL_RED;
        }
        else if (nrComponents == 3)
        {
            format = GL_RGB;
        }
        else if (nrComponents == 4)
        {
            format = GL_RGBA;
        }
        else
        {
            std::cout << "Imaget at path can't be find the texture color format: " << path << std::endl;
            return -1;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        
        return textureID;
    }

    void Primitives::init(RenderSystem* system)
    {
        constexpr const char* vsFile = "C:\\Users\\j.iznardo\\Documents\\Aiko\\assets\\shaders\\aiko_primitives.vs";
        constexpr const char* fsFile = "C:\\Users\\j.iznardo\\Documents\\Aiko\\assets\\shaders\\aiko_primitives.fs";
        shader = system->createShader(vsFile, fsFile);
        renderSystem = system;
    }

    void Primitives::use()
    {
        glDisable(GL_CULL_FACE);
        glUseProgram(shader->getData()->id);
    }

    void Primitives::unuse()
    {
        glUseProgram(0);
        glEnable(GL_CULL_FACE);
    }

    void Primitives::bindData(std::vector<float>& verts)
    {

        std::vector<Data::Vertex> vertices;
        for (size_t idx = 0; idx < verts.size(); idx += 3)
        {
            float x = verts[idx + 0];
            float y = verts[idx + 1];
            float z = verts[idx + 2];
            vec3 position = { x, y, z };
            vec3 normal = { 0, 0, 0 };
            vec2 uv = { 0, 0 };
            vertices.push_back({ position, normal, uv });
        }
        Data data = { vertices };
        calculateNormals(data);
        calculateUvs(data);

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

    void Primitives::calculateNormals(Data& data)
    {

        if (data.vertices.size() < 3)
        {
            return;
        }

        for (size_t i = 0; i + 2 < data.vertices.size(); i += 3)
        {

            size_t a = (i + 0);
            size_t b = (i + 1);
            size_t c = (i + 2);

            vec3 v0 = data.vertices[a].position;
            vec3 v1 = data.vertices[b].position;
            vec3 v2 = data.vertices[c].position;

            vec3 edge1 = v1 - v0;
            vec3 edge2 = v2 - v0;
            auto cross = math::cross(edge1, edge2);
            vec3 normal = math::normalize( cross );

            data.vertices[a].normal = data.vertices[a].normal + normal;
            data.vertices[b].normal = data.vertices[b].normal + normal;
            data.vertices[c].normal = data.vertices[c].normal + normal;

        }

        for (size_t i = 0; i < data.vertices.size(); ++i)
        {
            data.vertices[i].normal = math::normalize(data.vertices[i].normal);
        }
    }

    void Primitives::calculateUvs(Data& data)
    {
        for (size_t i = 0; i < data.vertices.size(); ++i)
        {
            // For planar mapping, use XY coordinates of the position as texture coordinates
            data.vertices[i].texCoords = vec2(data.vertices[i].position.x, data.vertices[i].position.y);
        }
    }

    void Primitives::bindShaderAttributes()
    {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }

    namespace light
    {
        enum LightType
        {
            Dir,
            Point,
            Spot,
        };

        struct Light
        {

            LightType type;

            vec3 position;
            vec3 direction;
            vec3 color;
            float intensity;

            float cutOff;
            float outerCutOff;

            vec3 ambient;
            vec3 diffuse;
            vec3 specular;

            float constant;
            float linear;
            float quadratic;

        };

        Light directionalLight()
        {
            Light light = {};
            light.type = LightType::Dir;
            light.intensity = 1.0f;
            light.color = { 1.0f };
            light.direction = { -0.2f, -1.0f, -0.3f };
            light.ambient = { 0.05f, 0.05f, 0.05f };
            light.diffuse = { 0.4f, 0.4f, 0.4f };
            light.specular = { 0.5f, 0.5f, 0.5f };
            return light;
        };

        Light pointLight()
        {
            Light light = {};
            light.type = LightType::Point;
            light.intensity = 1.0f;
            light.color = { 1.0f };
            light.position = { 0.0f, 0.0f, 0.0f };
            light.ambient = { 0.05f, 0.05f, 0.05f };
            light.diffuse = { 0.4f, 0.4f, 0.4f };
            light.specular = { 0.5f, 0.5f, 0.5f };
            light.constant = 1.0f;
            light.linear = 0.09f;
            light.quadratic = 0.032f;
            return light;
        };

        Light spotLight()
        {
            Light light = {};
            light.type = LightType::Spot;
            light.intensity = 1.0f;
            light.color = { 1.0f };
            light.position = { 0.0f, 0.0f, 0.0f };
            light.direction = { -0.2f, 1.0f, -0.3f };
            light.ambient = { 0.0f, 0.0f, 0.0f };
            light.diffuse = { 1.0f, 1.0f, 1.0f };
            light.specular = { 1.0f, 1.0f, 1.0f };
            light.constant = 1.0f;
            light.linear = 0.09f;
            light.quadratic = 0.032f;
            light.cutOff = math::cos(math::radians(12.5f));
            light.outerCutOff = math::cos(math::radians(15.0f));
            return light;
        };

    }

    void Primitives::setUniforms(vec4 color)
    {

        static std::vector<light::Light> s_lights =
        {
            light::directionalLight(),
            light::pointLight(),
            light::spotLight(),
        };

        Camera* cam = renderSystem->getMainCamera();

        shader->setFloat("ambientStrength", 1.0f);
        shader->setVec3("ambientColor", { 1.0f, 1.0f, 1.0f });
        shader->setInt("numLights", s_lights.size());

        shader->setVec3("camPos", cam->position);

        shader->setVec4("material.color", color);
        shader->setFloat("material.shininess", 32.0f);
        // shader->setInt("material.diffuse", 0);
        // shader->setInt("material.specular", 1);

        for (int i = 0; i < s_lights.size(); i++)
        {

            light::Light& light = s_lights[i];

            const auto l = string_format("lights[%s].", std::to_string(i).c_str());

            shader->setVec3(l + "type", light.type);
            shader->setVec3(l + "position", light.position);
            shader->setVec3(l + "direction", light.direction);
            shader->setVec3(l + "color", light.color);
            shader->setFloat(l + "intensity", light.intensity);

            shader->setVec3(l + "ambient", light.ambient);
            shader->setVec3(l + "diffuse", light.diffuse);
            shader->setVec3(l + "specular", light.specular);

            shader->setFloat(l + "constant", light.constant);
            shader->setFloat(l + "linear", light.linear);
            shader->setFloat(l + "quadratic", light.quadratic);

            shader->setFloat(l + "cutOff", light.cutOff);
            shader->setFloat(l + "outerCutOff", light.outerCutOff);

        }

        auto projection = cam->getProjectionMatrix();
        shader->setMat4("projection", projection);

        auto view = cam->getViewMatrix();
        shader->setMat4("view", view);

        Transform trans;
        shader->setMat4("model", trans.getMatrix());
    }

    // 2D

    void Primitives::drawPoint(vec3 pos, vec4 color)
    {

        use();

        // Define point vertices
        std::vector<GLfloat> pointVertex =
        {
            pos.x, pos.y, pos.z
        };

        // Create and bind VAO and VBO
        GLuint VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        bindData(pointVertex);
        bindShaderAttributes();
        setUniforms(color);

        // Draw point
        glDrawArrays(GL_POINTS, 0, 1);

        // Unbind VAO and VBO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Delete VAO and VBO
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);

        unuse();
    }

    void Primitives::drawTriangle(vec3 pos, float radius, vec4 color)
    {
        float height = sqrt(3.0f) * radius / 2.0f;
        vec3 a = { pos.x, pos.y + height / 2.0f, pos.z };
        vec3 b = { pos.x - radius / 2.0f, pos.y - height / 2.0f, pos.z };
        vec3 c = { pos.x + radius / 2.0f, pos.y - height / 2.0f, pos.z };
        drawTriangle(a, b, c);
    }

    void Primitives::drawTriangle(vec3 a, vec3 b, vec3 c, vec4 color)
    {
        use();

        std::vector<GLfloat> vertices =
        {
            a.x, a.y, a.z,
            b.x, b.y, b.z,
            c.x, c.y, c.z,
        };

        // Create and bind VAO and VBO
        GLuint VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        bindData(vertices);
        bindShaderAttributes();
        setUniforms(color);

        // Draw triangle
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Unbind VAO and VBO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Delete VAO and VBO
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);

        unuse();
    }

    void Primitives::drawRectangle(vec3 pos, vec3 size, vec4 color)
    {

        use();

        std::vector<GLfloat> rectangleVertices =
        {
            pos.x + size.x, pos.y + size.y, pos.z,
            pos.x, pos.y + size.y, pos.z,
            pos.x, pos.y, pos.z,
            pos.x, pos.y, pos.z,
            pos.x + size.x, pos.y, pos.z,
            pos.x + size.x, pos.y + size.y, pos.z,
        };

        // Create and bind VAO and VBO
        GLuint VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        bindData(rectangleVertices);
        bindShaderAttributes();
        setUniforms(color);

        // Draw rectangle
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Unbind VAO and VBO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Delete VAO and VBO
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);

        unuse();

    }

    void Primitives::renderLine(vec3 start, vec3 end, vec4 color)
    {

        use();

        // Create vertex array object (VAO) and vertex buffer object (VBO)
        GLuint VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        // Bind VAO and VBO
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // Upload vertex data to VBO
        std::vector<GLfloat> cubeVertices = { start.x, start.y, start.z, end.x, end.y, end.z };
        bindData(cubeVertices);

        bindShaderAttributes();
        setUniforms(color);

        // Draw line
        glDrawArrays(GL_LINES, 0, 2);

        // Clean up VAO and VBO
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);

        unuse();

    }

    void Primitives::renderCircle(vec3 pos, float radius, int numSegments, vec4 color)
    {

        use();

        // Create vertex array object (VAO) and vertex buffer object (VBO)
        GLuint VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        // Bind VAO and VBO
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // Calculate vertices for the circle
        std::vector<GLfloat> vertices;
        vertices.push_back(pos.x); // Center x
        vertices.push_back(pos.y); // Center y
        vertices.push_back(pos.z); // Center z

        for (int i = 0; i <= numSegments; ++i)
        {
            float angle = 2.0f * PI * i / numSegments;
            vertices.push_back(pos.x + radius * cos(angle)); // x
            vertices.push_back(pos.y + radius * sin(angle)); // y
            vertices.push_back(pos.z); // z
        }

        bindData(vertices);
        bindShaderAttributes();
        setUniforms(color);

        // Draw circle
        glDrawArrays(GL_TRIANGLE_FAN, 0, numSegments + 2);

        // Clean up VAO and VBO
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);

        unuse();

    }

    void Primitives::renderNgon(vec3 pos, float size, unsigned int ngon, vec4 color)
    {
        use();

        // Calculate angle between vertices
        float angleIncrement = 2.0f * PI / ngon;

        // Define vertex array
        std::vector<GLfloat> vertices;
        vertices.reserve(3 * ngon);

        // Generate vertices
        for (unsigned int i = 0; i < ngon; ++i)
        {
            float angle = i * angleIncrement;
            float x = pos.x + size * cos(angle);
            float y = pos.y + size * sin(angle);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(pos.z);
        }

        // Create and bind VAO and VBO
        GLuint VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        bindData(vertices);
        bindShaderAttributes();
        setUniforms(color);

        // Draw ngon
        glDrawArrays(GL_TRIANGLE_FAN, 0, ngon);

        // Unbind VAO and VBO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Delete VAO and VBO
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);

        unuse();
    }

    // 3D

    void Primitives::drawPyramid(vec3 pos, float baseWidth, float height, vec4 color)
    {
        use();

        // Calculate half base width
        float halfBaseWidth = baseWidth / 2.0f;

        // Define pyramid vertices

        std::vector<GLfloat> pyramidVertices =
        {
            // Base vertices (in a counterclockwise order)
            pos.x - halfBaseWidth, pos.y, pos.z - halfBaseWidth,  // Bottom left
            pos.x + halfBaseWidth, pos.y, pos.z - halfBaseWidth,  // Bottom right
            pos.x + halfBaseWidth, pos.y, pos.z + halfBaseWidth,  // Top right

            pos.x + halfBaseWidth, pos.y, pos.z + halfBaseWidth,  // Top right
            pos.x - halfBaseWidth, pos.y, pos.z + halfBaseWidth,  // Top left
            pos.x - halfBaseWidth, pos.y, pos.z - halfBaseWidth,  // Bottom left

            // Side triangles
            pos.x, pos.y + height, pos.z,                        // Apex
            pos.x - halfBaseWidth, pos.y, pos.z - halfBaseWidth,  // Bottom left
            pos.x + halfBaseWidth, pos.y, pos.z - halfBaseWidth,  // Bottom right

            pos.x, pos.y + height, pos.z,                        // Apex
            pos.x + halfBaseWidth, pos.y, pos.z - halfBaseWidth,  // Bottom right
            pos.x + halfBaseWidth, pos.y, pos.z + halfBaseWidth,  // Top right

            pos.x, pos.y + height, pos.z,                        // Apex
            pos.x + halfBaseWidth, pos.y, pos.z + halfBaseWidth,  // Top right
            pos.x - halfBaseWidth, pos.y, pos.z + halfBaseWidth,  // Top left

            pos.x, pos.y + height, pos.z,                        // Apex
            pos.x - halfBaseWidth, pos.y, pos.z + halfBaseWidth,  // Top left
            pos.x - halfBaseWidth, pos.y, pos.z - halfBaseWidth   // Bottom left
        };

        // Create and bind VAO and VBO
        GLuint VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        bindData(pyramidVertices);
        bindShaderAttributes();
        setUniforms(color);

        // Draw pyramid
        glDrawArrays(GL_TRIANGLES, 0, 18); // Draw 18 vertices (6 triangles)

        // Unbind VAO and VBO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Delete VAO and VBO
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);

        unuse();
    }

    void Primitives::drawCube(vec3 pos, vec3 size, vec4 color)
    {
        use();

        // Calculate half size
        vec3 halfSize = size / 2.0f;

        // Define cube vertices
        std::vector<GLfloat> cubeVertices =
        {
            // Front face
            pos.x - halfSize.x, pos.y - halfSize.y, pos.z + halfSize.z,
            pos.x + halfSize.x, pos.y - halfSize.y, pos.z + halfSize.z,
            pos.x + halfSize.x, pos.y + halfSize.y, pos.z + halfSize.z,
            pos.x + halfSize.x, pos.y + halfSize.y, pos.z + halfSize.z,
            pos.x - halfSize.x, pos.y + halfSize.y, pos.z + halfSize.z,
            pos.x - halfSize.x, pos.y - halfSize.y, pos.z + halfSize.z,
            // Back face
            pos.x - halfSize.x, pos.y - halfSize.y, pos.z - halfSize.z,
            pos.x + halfSize.x, pos.y - halfSize.y, pos.z - halfSize.z,
            pos.x + halfSize.x, pos.y + halfSize.y, pos.z - halfSize.z,
            pos.x + halfSize.x, pos.y + halfSize.y, pos.z - halfSize.z,
            pos.x - halfSize.x, pos.y + halfSize.y, pos.z - halfSize.z,
            pos.x - halfSize.x, pos.y - halfSize.y, pos.z - halfSize.z,
            // Left face                       y                .z
            pos.x - halfSize.x, pos.y - halfSize.y, pos.z + halfSize.z,
            pos.x - halfSize.x, pos.y + halfSize.y, pos.z + halfSize.z,
            pos.x - halfSize.x, pos.y + halfSize.y, pos.z - halfSize.z,
            pos.x - halfSize.x, pos.y + halfSize.y, pos.z - halfSize.z,
            pos.x - halfSize.x, pos.y - halfSize.y, pos.z - halfSize.z,
            pos.x - halfSize.x, pos.y - halfSize.y, pos.z + halfSize.z,
            // Right face                      y                .z
            pos.x + halfSize.x, pos.y - halfSize.y, pos.z + halfSize.z,
            pos.x + halfSize.x, pos.y + halfSize.y, pos.z + halfSize.z,
            pos.x + halfSize.x, pos.y + halfSize.y, pos.z - halfSize.z,
            pos.x + halfSize.x, pos.y + halfSize.y, pos.z - halfSize.z,
            pos.x + halfSize.x, pos.y - halfSize.y, pos.z - halfSize.z,
            pos.x + halfSize.x, pos.y - halfSize.y, pos.z + halfSize.z,
            // Top face                        y                .z
            pos.x - halfSize.x, pos.y + halfSize.y, pos.z + halfSize.z,
            pos.x + halfSize.x, pos.y + halfSize.y, pos.z + halfSize.z,
            pos.x + halfSize.x, pos.y + halfSize.y, pos.z - halfSize.z,
            pos.x + halfSize.x, pos.y + halfSize.y, pos.z - halfSize.z,
            pos.x - halfSize.x, pos.y + halfSize.y, pos.z - halfSize.z,
            pos.x - halfSize.x, pos.y + halfSize.y, pos.z + halfSize.z,
            // Bottom face                     y                .z
            pos.x - halfSize.x, pos.y - halfSize.y, pos.z + halfSize.z,
            pos.x + halfSize.x, pos.y - halfSize.y, pos.z + halfSize.z,
            pos.x + halfSize.x, pos.y - halfSize.y, pos.z - halfSize.z,
            pos.x + halfSize.x, pos.y - halfSize.y, pos.z - halfSize.z,
            pos.x - halfSize.x, pos.y - halfSize.y, pos.z - halfSize.z,
            pos.x - halfSize.x, pos.y - halfSize.y, pos.z + halfSize.z
        };

        // Create and bind VAO and VBO
        GLuint VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        bindData(cubeVertices);

        // Bind shader attributes
        bindShaderAttributes();
        setUniforms(color);

        // Draw cube
        glDrawArrays(GL_TRIANGLES, 0, 36); // Draw 36 vertices (12 triangles)

        // Unbind VAO and VBO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Delete VAO and VBO
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);

        unuse();
    }

    void Primitives::renderSphere(vec3 pos, float radius, int numSegments, vec4 color)
    {
        use();

        // Define sphere vertices
        const float pi = PI;
        std::vector<GLfloat> sphereVertices;

        int slices = numSegments;                // Number of slices (longitude divisions)
        int stacks = numSegments;

        for (int stack = 0; stack <= stacks; ++stack) {
            float phi = pi * stack / stacks;
            float phiNext = pi * (stack + 1) / stacks;

            for (int slice = 0; slice <= slices; ++slice) {
                float theta = 2 * pi * slice / slices;
                float thetaNext = 2 * pi * (slice + 1) / slices;

                // Vertex positions
                float x0 = radius * std::sin(phi) * std::cos(theta);
                float y0 = radius * std::sin(phi) * std::sin(theta);
                float z0 = radius * std::cos(phi);

                float x1 = radius * std::sin(phi) * std::cos(thetaNext);
                float y1 = radius * std::sin(phi) * std::sin(thetaNext);
                float z1 = radius * std::cos(phi);

                float x2 = radius * std::sin(phiNext) * std::cos(theta);
                float y2 = radius * std::sin(phiNext) * std::sin(theta);
                float z2 = radius * std::cos(phiNext);

                float x3 = radius * std::sin(phiNext) * std::cos(thetaNext);
                float y3 = radius * std::sin(phiNext) * std::sin(thetaNext);
                float z3 = radius * std::cos(phiNext);

                // Add triangle vertices
                sphereVertices.push_back( pos.x + x0);
                sphereVertices.push_back( pos.y + y0);
                sphereVertices.push_back( pos.z + z0);

                sphereVertices.push_back( pos.x + x1);
                sphereVertices.push_back( pos.y + y1);
                sphereVertices.push_back( pos.z + z1);

                sphereVertices.push_back( pos.x + x2);
                sphereVertices.push_back( pos.y + y2);
                sphereVertices.push_back( pos.z + z2);

                sphereVertices.push_back( pos.x + x1);
                sphereVertices.push_back( pos.y + y1);
                sphereVertices.push_back( pos.z + z1);

                sphereVertices.push_back( pos.x + x3);
                sphereVertices.push_back( pos.y + y3);
                sphereVertices.push_back( pos.z + z3);

                sphereVertices.push_back( pos.x + x2);
                sphereVertices.push_back( pos.y + y2);
                sphereVertices.push_back( pos.z + z2);
            }
        }

        // Create and bind VAO and VBO
        GLuint VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        bindData(sphereVertices);

        // Bind shader attributes
        bindShaderAttributes();
        setUniforms(color);

        // Draw sphere
        glDrawArrays(GL_TRIANGLES, 0, sphereVertices.size() / 3); // Divide by 3 because each vertex has 3 components

        // Unbind VAO and VBO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Delete VAO and VBO
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);

        unuse();

    }

    void Primitives::renderPolygon(vec3 pos, float radius, int rings, int sectors, vec4 color)
    {
        use();

        std::vector<GLfloat> sphereVertices;
        float const R = 1.0f / (float)(rings - 1);
        float const S = 1.0f / (float)(sectors - 1);
        int r, s;

        for (r = 0; r < rings; ++r) {
            for (s = 0; s < sectors; ++s) {
                float phi = PI * r * R;
                float theta = TWO_PI * s * S;
                float x = std::cos(theta) * std::sin(phi);
                float y = std::sin(theta) * std::sin(phi);
                float z = std::cos(phi);

                sphereVertices.push_back(pos.x + radius * x);
                sphereVertices.push_back(pos.y + radius * y);
                sphereVertices.push_back(pos.z + radius * z);

                phi = PI * (r + 1) * R;
                x = std::cos(theta) * std::sin(phi);
                y = std::sin(theta) * std::sin(phi);
                z = std::cos(phi);

                sphereVertices.push_back(pos.x + radius * x);
                sphereVertices.push_back(pos.y + radius * y);
                sphereVertices.push_back(pos.z + radius * z);

                theta = TWO_PI * (s + 1) * S;
                x = std::cos(theta) * std::sin(phi);
                y = std::sin(theta) * std::sin(phi);
                z = std::cos(phi);

                sphereVertices.push_back(pos.x + radius * x);
                sphereVertices.push_back(pos.y + radius * y);
                sphereVertices.push_back(pos.z + radius * z);


                sphereVertices.push_back(pos.x + radius * x);
                sphereVertices.push_back(pos.y + radius * y);
                sphereVertices.push_back(pos.z + radius * z);

                phi = PI * r * R;
                x = std::cos(theta) * std::sin(phi);
                y = std::sin(theta) * std::sin(phi);
                z = std::cos(phi);

                sphereVertices.push_back(pos.x + radius * x);
                sphereVertices.push_back(pos.y + radius * y);
                sphereVertices.push_back(pos.z + radius * z);

                theta = TWO_PI * s * S;
                x = std::cos(theta) * std::sin(phi);
                y = std::sin(theta) * std::sin(phi);
                z = std::cos(phi);

                sphereVertices.push_back(pos.x + radius * x);
                sphereVertices.push_back(pos.y + radius * y);
                sphereVertices.push_back(pos.z + radius * z);
            }
        }

        // Create and bind VAO and VBO
        GLuint VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        bindData(sphereVertices);

        // Bind shader attributes
        bindShaderAttributes();
        setUniforms(color);

        // Draw sphere
        glDrawArrays(GL_TRIANGLES, 0, sphereVertices.size() / 3);

        // Unbind VAO and VBO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Delete VAO and VBO
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);

        unuse();
    }

    void Primitives::renderCylinder(vec3 pos, float radius, float height, int sectors, vec4 color)
    {
        use();

        std::vector<GLfloat> cylinderVertices;
        float const sectorStep = 2 * PI / sectors;

        // Generate vertices for the side of the cylinder
        for (int i = 0; i <= sectors; ++i) {
            float x = radius * std::cos(i * sectorStep);
            float z = radius * std::sin(i * sectorStep);

            // Bottom vertex
            cylinderVertices.push_back(pos.x + x);
            cylinderVertices.push_back(pos.y);
            cylinderVertices.push_back(pos.z + z);

            // Top vertex
            cylinderVertices.push_back(pos.x + x);
            cylinderVertices.push_back(pos.y + height);
            cylinderVertices.push_back(pos.z + z);
        }

        // Create and bind VAO and VBO
        GLuint VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        bindData(cylinderVertices);

        // Bind shader attributes
        bindShaderAttributes();
        setUniforms(color);

        // Draw the sides of the cylinder
        glDrawArrays(GL_TRIANGLE_STRIP, 0, cylinderVertices.size() / 3);

        // Unbind VAO and VBO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Delete VAO and VBO
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);

        unuse();
    }

}
