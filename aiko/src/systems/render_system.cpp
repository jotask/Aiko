#include "entity_component_system.h"

#include <stdexcept>
#include <memory>
#include <cassert>
#include <iostream>
#include <format>
#include <iostream>

#include "aiko_types.h"
#include "shared/math.h"
#include "modules/module_connector.h"
#include "modules/camera_module.h"
#include "components/transform_component.h"
#include "components/component_renderer.h"
#include "components/mesh_component.h"
#include "render_system.h"
#include "models/light.h"
#include "models/mesh_factory.h"

#include "modules/render_primitives.h"
#include "modules/render_primitives.h"

// DELETEME
#include <GLFW/glfw3.h>

namespace aiko
{

    RenderSystem::RenderSystem()
    {
    }
    
    void RenderSystem::init()
    {
        Primitives::init(this);
    }

    void RenderSystem::update()
    {

    }

    void RenderSystem::render()
    {

    }

    aiko::AikoPtr<Mesh> RenderSystem::createMesh()
    {
        auto mesh = std::make_unique<Mesh>();
        mesh::generatTest(*mesh.get());
        m_renderModule->initMesh(mesh.get());
        return mesh;
    }
    
    aiko::AikoPtr<Light> RenderSystem::createLight()
    {
        auto light = std::make_unique<Light>();
        return light;
    }
    
    void RenderSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_MODULE_REQUIRED(RenderModule, moduleConnector, m_renderModule)
        BIND_MODULE_REQUIRED(CameraModule, moduleConnector, m_cameraModule)
    }
    
    void RenderSystem::add(Light* light)
    {
        switch (light->m_type)
        {
        case Light::Type::Ambient:
            {
    
            }
            break;
        case Light::Type::Directional:
            {
    
            }
            break;
        case Light::Type::Point:
            {
    
            }
            break;
        default:
            throw new std::exception();
            break;
        }
    }
   
    void RenderSystem::render(MeshComponent* mesh)
    {
        // m_renderModule->renderMesh(getMainCamera() , mesh->gameobject->transform().get(), mesh->m_mesh.get(), mesh->m_shader.get());
    }

    texture::RenderTexture2D* RenderSystem::getTargetTexture() const
    {
        return m_renderModule->getRenderTexture();
    }

    void RenderSystem::render(texture::RenderTexture2D& target, Shader* shader)
    {

        m_renderModule->beginShaderMode(shader);

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
        {

            Camera* cam = this->getMainCamera();

            auto projection = cam->getProjectionMatrix();
            shader->setMat4("projection", projection);

            auto view = cam->getViewMatrix();
            shader->setMat4("view", view);

            Transform trans;
            shader->setMat4("model", trans.getMatrix());

        }

        // Draw rectangle
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Unbind VAO and VBO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Delete VAO and VBO
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);

        m_renderModule->endShaderMode();
        glEnable(GL_CULL_FACE);

    }

    AikoUPtr<Shader> RenderSystem::createShader()
    {
        return m_renderModule->createShader();
    }

    AikoUPtr<Shader> RenderSystem::createShader(const char* vsPath, const char* fsPath)
    {
        auto shader = createShader();
        shader->load( vsPath, fsPath );
        return shader;
    }

    void RenderSystem::unloadShader(Shader& data)
    {
        m_renderModule->unloadShader(data.m_shaderData);
    }

    int RenderSystem::getShaderLocation(Shader& shader, const char* uniformName)
    {
        return m_renderModule->getShaderLocation( shader.m_shaderData, uniformName );
    }

    void RenderSystem::setShaderUniformValue(Shader& shader, int locIndex, const void* value, aiko::ShaderUniformDataType uniformType)
    {
        m_renderModule->setShaderUniformValue(shader.m_shaderData, locIndex, value, uniformType);
    }

    void RenderSystem::setShaderUniformValueV(Shader& shader, int locIndex, const void* value, aiko::ShaderUniformDataType uniformType, int count)
    {
        m_renderModule->setShaderUniformValueV(shader.m_shaderData, locIndex, value, uniformType, count);
    }

    Camera* RenderSystem::getMainCamera()
    {
        return m_cameraModule->GetMainCamera();
    }

}
