#include "entity_component_system.h"

#include <stdexcept>
#include <memory>
#include <cassert>
#include <iostream>
#include <format>
#include <iostream>

#include "aiko_types.h"
#include "modules/module_connector.h"
#include "modules/scene_module.h"
#include "components/transform_component.h"
#include "components/component_renderer.h"
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
        mesh->m_vertices = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f
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

    }

    texture::RenderTexture2D* RenderSystem::getTargetTexture() const
    {
        return m_renderModule->getRenderTexture();
    }

    void RenderSystem::render(texture::RenderTexture2D& target, aiko::asset::Shader* shader)
    {
        // m_renderModule->beginShaderMode(&shader->m_shaderData);
        // m_renderModule->drawTextureEx(target.texture, { 0.0f, 0.0f }, 0.0f, 1.0f, WHITE);
        // m_renderModule->endShaderMode();
    }

    aiko::ShaderData RenderSystem::loadShaderData(const char* vs, const char* fs)
    {
        // return m_renderModule->loadShaderData(vs, fs);
        return {};
    }

    void RenderSystem::unloadShader(asset::Shader& data)
    {
        // m_renderModule->unloadShader(data.m_shaderData);
    }

    int RenderSystem::getShaderLocation(asset::Shader& shader, const char* uniformName)
    {
        // return m_renderModule->getShaderLocation( shader.m_shaderData, uniformName );
        return -1;
    }

    void RenderSystem::setShaderUniformValue(asset::Shader& shader, int locIndex, const void* value, aiko::ShaderUniformDataType uniformType)
    {
        // m_renderModule->setShaderUniformValue(shader.m_shaderData, locIndex, value, uniformType);
    }

    void RenderSystem::setShaderUniformValueV(asset::Shader& shader, int locIndex, const void* value, aiko::ShaderUniformDataType uniformType, int count)
    {
        // m_renderModule->setShaderUniformValueV(shader.m_shaderData, locIndex, value, uniformType, count);
    }

}
