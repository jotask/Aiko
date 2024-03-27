#include "entity_component_system.h"

#include <stdexcept>
#include <memory>
#include <cassert>
#include <iostream>
#include <format>
#include <iostream>

#include "aiko_types.h"
#include "modules/module_connector.h"
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
            // positions                // texture coords
             0.5f,  0.5f, 0.0f,         1.0f, 1.0f,         // top right
             0.5f, -0.5f, 0.0f,         1.0f, 0.0f,         // bottom right
            -0.5f, -0.5f, 0.0f,         0.0f, 0.0f,         // bottom left
            -0.5f,  0.5f, 0.0f,         0.0f, 1.0f          // top left 
        };

        mesh->m_indices =
        {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
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
        m_renderModule->renderMesh( mesh->gameobject->transform().get(), mesh->m_mesh.get(), mesh->m_shader.get());
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
