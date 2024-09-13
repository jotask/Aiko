#include "asset_system.h"

#include <algorithm>

#include "modules/module_connector.h"
#include "systems/system_connector.h"

#include "modules/render/render_module.h"
#include "components/transform_component.h"
#include "components/camera_component.h"
#include "types/camera_types.h"
#include "models/camera.h"
#include "models/shader.h"
#include "systems/render_system.h"

namespace aiko
{

    void AssetSystem::unload(asset::Asset* asset)
    {
        this->unload(asset->getID());
    }

    void AssetSystem::unload(asset::ID asset)
    {
        auto it = std::find_if(m_assets.begin(), m_assets.end(), [&](const AikoPtr<asset::Asset>& ptr) {
            return ptr->getID() == asset;
            });
        if (it != m_assets.end())
        {
            (*it)->unload();
            (*it).reset(); // Call destructor
            m_assets.erase(it);
        }
    }

    void AssetSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_SYSTEM_REQUIRED(RenderSystem, systemConnector, m_renderSystem)
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
