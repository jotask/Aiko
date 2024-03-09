#include "asset_system.h"

#include "modules/module_connector.h"
#include "systems/system_connector.h"

#include "modules/render_module.h"
#include "modules/scene_module.h"
#include "components/transform_component.h"
#include "components/camera_component.h"
#include "types/camera_types.h"
#include "models/camera.h"
#include "models/shader.h"
#include "systems/render_system.h"

namespace aiko
{
    asset::Shader* AssetSystem::loadShader(const char* vs, const char* fs)
    {
        auto shared = std::make_unique<asset::Shader>();
        initAsset(shared.get());
        ((asset::Asset*)shared.get())->connect();
        shared->load(vs, fs);
        asset::ID uuid = shared->getID();
        m_assets[uuid] = std::move(shared);
        return (asset::Shader*)m_assets[uuid].get();
    }

    void AssetSystem::unload(asset::Asset& asset)
    {
        this->unload(asset.getID());
    }

    void AssetSystem::unload(asset::ID asset)
    {
        if (asset == asset::INVALID_ID)
        {
            // Already deleted?
            return;
        }
        auto it = m_assets.find(asset);
        if (it != m_assets.end())
        {
            (*it).second.get()->unload(); // unload 
            (*it).second.reset(); // Call destructor
            m_assets.erase(it);
        }
    }

    void AssetSystem::initAsset(asset::Asset* asset)
    {
        asset->m_assetSystem = this;
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
