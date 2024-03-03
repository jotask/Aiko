#pragma once

#include <map>

#include "aiko_types.h"
#include "modules/base_module.h"
#include "types/asset_type.h"

namespace aiko
{

    class AssetModule : public BaseModule
    {
    public:

        AssetModule() = default;
        virtual ~AssetModule() = default;

        virtual void init() override;

        asset::TextAsset* loadTextAsset(char* filePath);

    protected:

        using Assets = std::map<asset::ID, AikoUPtr<asset::Asset>>;

        Assets m_assets;

    };

}
