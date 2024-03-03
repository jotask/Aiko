#include "asset_system.h"

#include "modules/module_connector.h"

#include "modules/assets/asset_module.h"
#include "modules/renderer/render_module.h"
#include "modules/scene_module.h"
#include "components/transform_component.h"
#include "components/camera_component.h"
#include "types/camera_types.h"
#include "models/camera.h"

namespace aiko
{

    void AssetSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_MODULE_REQUIRED(AssetModule, moduleConnector, m_assetModule)
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
