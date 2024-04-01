#include "render_primitives.h"

#include "shared/math.h"
#include "systems/render_system.h"
#include "models/camera.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace aiko
{

    AikoPtr<Shader> Primitives::shader = nullptr;
    RenderSystem* Primitives::renderSystem = nullptr;

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

    void Primitives::bindShaderAttributes()
    {
        // Set vertex attribute pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

    }

    void Primitives::setUniforms(vec4 color)
    {

        shader->setVec4("color", color );

        Camera* cam = renderSystem->getMainCamera();

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
        GLfloat pointVertex[] =
        {
            pos.x, pos.y, pos.z
        };

        // Create and bind VAO and VBO
        GLuint VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pointVertex), pointVertex, GL_STATIC_DRAW);

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

        GLfloat vertices[] = {
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
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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

        // Define rectangle vertices
        GLfloat rectangleVertices[] =
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
        glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

        // Bind shader attributes
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
        GLfloat vertices[] = { start.x, start.y, start.z, end.x, end.y, end.z };
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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

        // Upload vertex data to VBO
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

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
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

        // Bind shader attributes
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
        GLfloat pyramidVertices[] = {
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
        glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);

        // Bind shader attributes
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
        GLfloat cubeVertices[] = {
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
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

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
        glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(GLfloat), sphereVertices.data(), GL_STATIC_DRAW);

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
        glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(GLfloat), sphereVertices.data(), GL_STATIC_DRAW);

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
        glBufferData(GL_ARRAY_BUFFER, cylinderVertices.size() * sizeof(GLfloat), cylinderVertices.data(), GL_STATIC_DRAW);

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
