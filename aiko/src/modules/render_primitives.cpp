#include "render_primitives.h"

#include "shared/math.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace aiko
{

    AikoPtr<Shader> Primitives::shader = nullptr;; //its a definition

    void Primitives::use()
    {
        // Use shader program
        glUseProgram(shader->getData()->id);
    }

    void Primitives::unuse()
    {
        glUseProgram(0);
    }

    void Primitives::bindShaderAttributes()
    {
        // Set vertex attribute pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        shader->setVec4("color", { 1.,0.686,0.8, 1.0f});

    }

    void Primitives::drawPoint(vec3 pos)
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

    void Primitives::drawTriangle(vec3 pos, float radius)
    {
        float height = sqrt(3.0f) * radius / 2.0f;
        vec3 a = { pos.x, pos.y + height / 2.0f, pos.z };
        vec3 b = { pos.x - radius / 2.0f, pos.y - height / 2.0f, pos.z };
        vec3 c = { pos.x + radius / 2.0f, pos.y - height / 2.0f, pos.z };
        drawTriangle(a, b, c);
    }

    void Primitives::drawTriangle(vec3 a, vec3 b, vec3 c)
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

    void Primitives::drawRectangle(vec3 pos, vec3 size)
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

    void Primitives::renderLine(vec3 start, vec3 end)
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

        // Draw line
        glDrawArrays(GL_LINES, 0, 2);

        // Clean up VAO and VBO
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);

        unuse();

    }

    void Primitives::renderCircle(vec3 pos, float radius, int numSegments)
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

        // Draw circle
        glDrawArrays(GL_TRIANGLE_FAN, 0, numSegments + 2);

        // Clean up VAO and VBO
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);

        unuse();

    }

    void Primitives::renderNgon(vec3 pos, float size, unsigned int ngon)
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

}
