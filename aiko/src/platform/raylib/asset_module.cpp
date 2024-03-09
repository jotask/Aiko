#ifdef AIKO_BUILD_RAYLIB
#include "modules/asset_module.h"

#include <memory>

#include "modules/render_module.h"
#include "modules/module_connector.h"

#include "platform/raylib/raylib_utils.h"
#include "core/libs.h"

namespace aiko
{
    void AssetModule::connect(ModuleConnector* moduleConnector)
    {
        BIND_MODULE_REQUIRED(RenderModule, moduleConnector, m_renderModule);
    }

    void AssetModule::init()
    {
        //constexpr char* path = "C:\\Users\\\j.iznardo\\Documents\\Aiko\\assets\\shaders\\aiko_shadertoy.fs";
        // auto data = loadTextAsset(path);
        // int a = 0;
    }

    asset::TextAsset* AssetModule::loadTextAsset(char* filePath)
    {
        const char* data = ::LoadFileText(filePath);
        auto shared = std::make_unique<asset::TextAsset>(data);
        initAsset(shared.get());
        asset::ID uuid = shared->getID();
        m_assets[uuid] = std::move( shared );
        return (asset::TextAsset*) m_assets[uuid].get();
    }

    asset::Shader* AssetModule::loadShader(const char* vs, const char* fs)
    {
        auto shared = std::make_unique<asset::Shader>();
        initAsset(shared.get());
        shared->load(vs, fs);
        asset::ID uuid = shared->getID();
        m_assets[uuid] = std::move(shared);
        return (asset::Shader*)m_assets[uuid].get();
    }

    void AssetModule::unload(asset::ID asset)
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

    RenderModule* AssetModule::getRenderModule()
    {
        return m_renderModule;
    }

    void AssetModule::initAsset(asset::Asset* asset)
    {
        // FIXME But it works
        asset->m_assetModule = this;
    }

}
#endif
