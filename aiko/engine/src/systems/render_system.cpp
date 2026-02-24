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
#include "modules/render_module.h"

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
        m_renderModule->setMainCamera(m_cameraSystem->getMainCamera());
    }

    void RenderSystem::render()
    {

    }

    void RenderSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_SYSTEM_REQUIRED(CameraSystem, systemConnector, m_cameraSystem)
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

    void RenderSystem::render( const Transform& trans, const Model& model)
    {
        for (const auto& m : model.m_meshes)
        {
            render(trans, m.mesh, m.material);
        }
    }

    void RenderSystem::render(const Transform& trans, const Mesh& mesh, const Material& mat)
    {
        AikoRenderer::it().submit(trans, mesh, mat);
    }

    FrameBuffer RenderSystem::getTargetTexture() const
    {
        return AikoRenderer::it().getTargetTexture();
    }

    Camera* RenderSystem::getMainCamera()
    {
        return m_cameraSystem->getMainCamera();
    }

    void RenderSystem::renderText(string str, float x, float y)
    {
        AIKO_NOT_IMPLEMENTED;
    }

}
