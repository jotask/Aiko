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
#include "models/mesh_factory.h"

#include "modules/render_primitives.h"

namespace aiko
{

    RenderSystem::RenderSystem()
    {
    }
    
    void RenderSystem::init()
    {
        Primitives::shader = this->createShader("C:\\Users\\j.iznardo\\Documents\\Aiko\\assets\\shaders\\aiko_primitives.vs", "C:\\Users\\j.iznardo\\Documents\\Aiko\\assets\\shaders\\aiko_primitives.fs");
    }

    void RenderSystem::update()
    {

    }

    void RenderSystem::render()
    {
        // Primitives::renderLine(vec3(-1, 0, 0), vec3(1, 0, 0));
        // Primitives::renderCircle(vec3(0, 0, 0), 0.1);
        // Primitives::drawPoint({0, 0.5, 0});
        // Primitives::drawTriangle({ 0.0f, -0.5f, 0.0f }, 0.25f);
        // Primitives::renderNgon(vec3(0.5f, 0.0f, 0.0f), 0.25f, 6);
        Primitives::drawRectangle(vec3(-0.5f, 0.0f, 0.0f), vec3(0.5f, 0.5f, 0.5f));
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
