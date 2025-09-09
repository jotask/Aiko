#include "render_system.h"

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
#include "components/mesh_component.h"
#include "systems/camera_system.h"
#include "models/light.h"
#include "models/mesh_factory.h"
#include "types/color.h"

#include "platform/bgfx/bgfx_platform_helper.h"

namespace aiko
{

    RenderSystem::RenderSystem()
    {
    }

    void RenderSystem::init()
    {
        this->initPrimitives();
    }

    void RenderSystem::update()
    {

    }

    void RenderSystem::render()
    {

    }

    Mesh RenderSystem::createMesh(Mesh::MeshType type)
    {
        auto mesh = Mesh();
        switch (type)
        {
        case aiko::Mesh::MeshType::CUBE:
            mesh::generateCube(mesh);
            break;
        case aiko::Mesh::MeshType::QUAD:
            mesh::generateQuad(mesh);
            break;
        case aiko::Mesh::MeshType::CUSTOM:
            break;
        default:
            break;
        }
        m_renderModule->refreshMesh(&mesh);
        return mesh;
    }
    
    aiko::AikoPtr<Light> RenderSystem::createLight()
    {
        auto light = std::make_unique<Light>();
        return light;
    }

    texture::Texture RenderSystem::createTexture()
    {
        return m_renderModule->createTexture();
    }

    texture::PboTexture RenderSystem::createPboTexture(uint16_t width, uint16_t height)
    {
        return m_renderModule->createPboTexture(width, height);
    }

    void RenderSystem::updatePbo(texture::PboTexture text, std::vector<Color>& pixels)
    {
        m_renderModule->updatePboTexture(text, pixels);
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
   
    void RenderSystem::render(Transform* trans, Mesh* mesh, Shader* shader)
    {
        m_renderModule->renderMesh(getMainCamera() , trans, mesh, shader);
    }

    void RenderSystem::render(Transform* trans, Mesh* mesh, Shader* shader, Texture* texture)
    {
        m_renderModule->renderMesh(getMainCamera(), trans, mesh, shader, texture);
    }

    void RenderSystem::render(Transform* trans, Mesh* mesh, Shader* shader, texture::PboTexture* texture)
    {
        m_renderModule->renderMesh(getMainCamera(), trans, mesh, shader, texture);
    }

    texture::RenderTexture2D* RenderSystem::getTargetTexture() const
    {
        return m_renderModule->getRenderTexture();
    }

    void RenderSystem::renderToFullScreen(Shader* shader)
    {
        m_renderModule->beginShaderMode(shader);
        m_renderModule->beginTextureMode();
        m_renderModule->beginMode2D();
        m_renderModule->endMode2D();
        m_renderModule->endTextureMode();
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

    Camera* RenderSystem::getMainCamera()
    {
        return m_cameraSystem->getMainCamera();
    }

    void RenderSystem::renderText(string str, float x, float y)
    {
        m_renderModule->drawText(str, x, y);
    }

}
