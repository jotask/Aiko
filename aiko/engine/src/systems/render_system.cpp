#include "render_system.h"

#include <stdexcept>
#include <memory>

#include <format>

#include <aiko_types.h>
#include <math/math.h>
#include <models/light.h>

#include "modules/module_connector.h"
#include "systems/system_connector.h"
#include "components/transform_component.h"
#include "components/mesh_component.h"
#include "systems/camera_system.h"

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

    void RenderSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
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

    void RenderSystem::render( const Transform& trans, const Model& model)
    {
        AikoRenderer::it().render(getMainCamera(), &trans, &model);
    }
   
    void RenderSystem::render(const Transform& trans, const Mesh& mesh, const Shader& shader)
    {
        AikoRenderer::it().render(getMainCamera(), &trans, &mesh, &shader);
    }

    void RenderSystem::render(const Transform& trans, const Mesh& mesh, const Shader& shader, const Texture& texture)
    {
        AikoRenderer::it().render(getMainCamera(), &trans, &mesh, &shader, &texture);
    }

    FrameBuffer RenderSystem::getTargetTexture() const
    {
        return AikoRenderer::it().getTargetTexture();
    }


    /*
    void RenderSystem::render(AikoPtr<FrameBuffer> target, AikoPtr<Shader> shader)
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
        AIKO_DEBUG_BREAK
        AIKO_NOT_IMPLEMENTED;
    }
    */

    Camera* RenderSystem::getMainCamera()
    {
        return m_cameraSystem->getMainCamera();
    }

    void RenderSystem::renderText(string str, float x, float y)
    {
        AIKO_NOT_IMPLEMENTED;
    }

}
