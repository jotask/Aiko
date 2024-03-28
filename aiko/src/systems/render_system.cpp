#include "entity_component_system.h"

#include <stdexcept>
#include <memory>
#include <cassert>
#include <iostream>
#include <format>
#include <iostream>

#include "aiko_types.h"
#include "modules/module_connector.h"
#include "modules/camera_module.h"
#include "components/transform_component.h"
#include "components/component_renderer.h"
#include "components/mesh_component.h"
#include "render_system.h"
#include "models/light.h"

namespace aiko
{

    RenderSystem::RenderSystem()
    {
    }
    
    void RenderSystem::init()
    {

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
        mesh->m_vertices =
        {
            // positions              // texture coords   // Color
            -0.5f,  0.5f,  0.5f,       0.0f, 1.0f,        1.0f, 0.0f, 0.0f, // Top-left
            -0.5f, -0.5f,  0.5f,       0.0f, 0.0f,        1.0f, 0.0f, 0.0f, // Bottom-left
             0.5f, -0.5f,  0.5f,       1.0f, 0.0f,        1.0f, 0.0f, 0.0f, // Bottom-right
             0.5f,  0.5f,  0.5f,       1.0f, 1.0f,        1.0f, 0.0f, 0.0f, // Top-right

            // Back face                                  
            -0.5f, -0.5f, -0.5f,       0.0f, 0.0f,         0.0f, 1.0f, 0.0f, // Bottom-left
            -0.5f,  0.5f, -0.5f,       0.0f, 1.0f,         0.0f, 1.0f, 0.0f, // Top-left
             0.5f,  0.5f, -0.5f,       1.0f, 1.0f,         0.0f, 1.0f, 0.0f, // Top-right
             0.5f, -0.5f, -0.5f,       1.0f, 0.0f,         0.0f, 1.0f, 0.0f, // Bottom-right

            // Top face                                   
            0.5f,  0.5f,  0.5f,       1.0f, 0.0f,         0.0f, 0.0f, 1.0f, // Front-top-right
            0.5f,  0.5f, -0.5f,       1.0f, 1.0f,         0.0f, 0.0f, 1.0f, // Back-top-right
            -0.5f,  0.5f, -0.5f,      0.0f, 1.0f,         0.0f, 0.0f, 1.0f, // Back-top-left
            -0.5f,  0.5f,  0.5f,      0.0f, 0.0f,         0.0f, 0.0f, 1.0f, // Front-top-left

            // Bottom face                                
            -0.5f, -0.5f, -0.5f,      0.0f, 1.0f,         1.0f, 1.0f, 0.0f, // Back-bottom- left
            0.5f, -0.5f, -0.5f,       1.0f, 1.0f,         1.0f, 1.0f, 0.0f, // Back-bottom-right
            0.5f, -0.5f,  0.5f,       1.0f, 0.0f,         1.0f, 1.0f, 0.0f, // Front-bottom-right
            -0.5f, -0.5f,  0.5f,      0.0f, 0.0f,         1.0f, 1.0f, 0.0f, // Front-bottom-left

            // Right face                                 
            0.5f, -0.5f,  0.5f,      1.0f, 0.0f,         1.0f, 0.0f, 1.0f, // Front-bottom-right
            0.5f, -0.5f, -0.5f,      1.0f, 1.0f,         1.0f, 0.0f, 1.0f, // Back-bottom-right
            0.5f,  0.5f, -0.5f,      0.0f, 1.0f,         1.0f, 0.0f, 1.0f, // Back-top-right
            0.5f,  0.5f,  0.5f,      0.0f, 0.0f,         1.0f, 0.0f, 1.0f, // Front-top-right

            // Left face                                  
            -0.5f,  0.5f,  0.5f,      0.0f, 1.0f,         0.0f, 1.0f, 1.0f, // Front-top-left
            -0.5f,  0.5f, -0.5f,      1.0f, 1.0f,         0.0f, 1.0f, 1.0f, // Back-top-left
            -0.5f, -0.5f, -0.5f,      1.0f, 0.0f,         0.0f, 1.0f, 1.0f, // Back-bottom-left
            -0.5f, -0.5f,  0.5f,      0.0f, 0.0f,         0.0f, 1.0f, 1.0f, // Front-bottom-left
        };

        mesh->m_indices =
        {
0, 1, 2, 2, 3, 0,       // Front face
    4, 5, 6, 6, 7, 4,       // Back face
    8, 9, 10, 10, 11, 8,    // Top face
    12, 13, 14, 14, 15, 12, // Bottom face
    16, 17, 18, 18, 19, 16, // Right face
    20, 21, 22, 22, 23, 20  // Left face
        };
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
        auto* cam = m_cameraModule->GetMainCamera();
        m_renderModule->renderMesh(cam, mesh->gameobject->transform().get(), mesh->m_mesh.get(), mesh->m_shader.get());
    }

    texture::RenderTexture2D* RenderSystem::getTargetTexture() const
    {
        return m_renderModule->getRenderTexture();
    }

    void RenderSystem::render(texture::RenderTexture2D& target, Shader* shader)
    {
        // m_renderModule->beginShaderMode(&shader->m_shaderData);
        // m_renderModule->drawTextureEx(target.texture, { 0.0f, 0.0f }, 0.0f, 1.0f, WHITE);
        // m_renderModule->endShaderMode();
    }

    aiko::AikoPtr<Shader> RenderSystem::createShader(const char* vsPath, const char* fsPath)
    {
        auto shader = std::make_unique<Shader>();
        m_renderModule->initShader(shader.get(), vsPath, fsPath);
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

}
