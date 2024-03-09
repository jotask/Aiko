#include "asset_system.h"

#include "modules/module_connector.h"

#include "modules/asset_module.h"
#include "modules/render_module.h"
#include "modules/scene_module.h"
#include "components/transform_component.h"
#include "components/camera_component.h"
#include "types/camera_types.h"
#include "models/camera.h"
#include "models/shader.h"
#include "modules/asset_module.h"

namespace aiko
{
    asset::Shader* AssetSystem::loadShader(const char* vs, const char* fs)
    {
        aiko::asset::Shader* shader = m_assetModule->loadShader(vs, fs);
        return shader;
    }

    void AssetSystem::unload(asset::Asset& asset)
    {
        this->unload(asset.getID());
    }

    void AssetSystem::unload(asset::ID id)
    {
        m_assetModule->unload(id);
    }

    void AssetSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_MODULE_REQUIRED(AssetModule, moduleConnector, m_assetModule)
        BIND_MODULE_REQUIRED(RenderModule, moduleConnector, m_renderModule)
    }
    
    void AssetSystem::init()
    {
    
    }
    
    void AssetSystem::update()
    {
    
    }
    
    void AssetSystem::render()
    {

    }

}
