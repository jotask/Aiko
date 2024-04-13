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
#include "systems/system_connector.h"
#include "components/transform_component.h"
#include "components/component_renderer.h"
#include "components/mesh_component.h"
#include "systems/camera_system.h"
#include "render_system.h"
#include "models/light.h"
#include "models/mesh_factory.h"
#include "types/color.h"

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
        BIND_SYSTEM_REQUIRED(CameraSystem, systemConnector, m_cameraSystem)
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

    void RenderSystem::renderToFullScreen(Shader* shader)
    {

        auto target = m_renderModule->getScreenTexture();
        m_renderModule->beginShaderMode(shader);

        Camera* cam = this->getMainCamera();

        auto projection = cam->getProjectionMatrix();
        shader->setMat4("projection", projection);

        auto view = cam->getViewMatrix();
        shader->setMat4("view", view);

        Transform trans;
        shader->setMat4("model", trans.getMatrix());

        m_renderModule->drawToScreenTexture();

        m_renderModule->endShaderMode();
    }

    void RenderSystem::render(texture::RenderTexture2D& target, Shader* shader)
    {
        m_renderModule->beginShaderMode(shader);

        Camera* cam = this->getMainCamera();

        auto projection = cam->getProjectionMatrix();
        shader->setMat4("projection", projection);

        auto view = cam->getViewMatrix();
        shader->setMat4("view", view);

        Transform trans;
        shader->setMat4("model", trans.getMatrix());

        m_renderModule->drawRenderTextureEx(target, vec2(), 0.0f, 1.0f, WHITE );

        m_renderModule->endShaderMode();
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
        return m_cameraSystem->getMainCamera();
    }

}
