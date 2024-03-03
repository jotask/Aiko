#ifdef AIKO_BUILD_RAYLIB
#include "modules/asset_module.h"

#include <memory>

#include "core/libs.h"

namespace aiko
{
    void AssetModule::init()
    {
        constexpr char* path = "C:\\Users\\\j.iznardo\\Documents\\Aiko\\assets\\shaders\\aiko_shadertoy.fs";
        auto data = loadTextAsset(path);
        int a = 0;
    }

    asset::TextAsset* AssetModule::loadTextAsset(char* filePath)
    {
        const char* data = ::LoadFileText(filePath);
        auto shared = std::make_unique<asset::TextAsset>(data);
        asset::ID uuid = shared->getID();
        m_assets[uuid] = std::move( shared );
        return (asset::TextAsset*) m_assets[uuid].get();
    }

}
#endif
