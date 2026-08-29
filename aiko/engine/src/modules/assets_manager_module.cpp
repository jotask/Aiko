#include "assets_manager_module.h"

#include "constants.h"

#include <filesystem>

namespace aiko
{

    namespace
    {
        const std::filesystem::path ASSET_REGISTRY_PATH = std::filesystem::path(global::GLOBAL_ASSET_PATH) / ".aiko" / "asset_registry.json";
    }

    AssetsManagerModule::AssetsManagerModule(Aiko* aiko)
       : BaseModule(aiko)
    {
    }

    void AssetsManagerModule::preInit()
    {
        if (std::filesystem::exists(ASSET_REGISTRY_PATH) == false)
        {
            return;
        }
        const bool loaded = m_manager.loadRegistry(ASSET_REGISTRY_PATH);
        AIKO_ASSERT(loaded, "Failed to load asset registry");
    }

    void AssetsManagerModule::init()
    {
    }

    void AssetsManagerModule::update()
    {
    }

    void AssetsManagerModule::beginFrame()
    {
    }

    void AssetsManagerModule::endFrame()
    {
    }

    void AssetsManagerModule::dispose()
    {
        const bool saved = m_manager.saveRegistry(ASSET_REGISTRY_PATH);
        AIKO_ASSERT(saved, "Failed to save asset registry");
        m_manager.clear();
    }

}
