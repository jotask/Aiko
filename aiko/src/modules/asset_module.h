#pragma once

#include <map>

#include "aiko_types.h"
#include "modules/base_module.h"
#include "types/asset_type.h"
#include "models/shader.h"

namespace aiko
{
    class RenderModule;
    class AssetModule : public BaseModule
    {
    public:

        AssetModule() = default;
        virtual ~AssetModule() = default;

        virtual void connect(ModuleConnector*) override;

        virtual void init() override;

        asset::TextAsset* loadTextAsset(char* filePath);

        asset::Shader* loadShader(const char* vs, const char* fs);

        void unload(asset::ID asset);

        RenderModule* getRenderModule();

    protected:

        using Assets = std::map<asset::ID, AikoUPtr<asset::Asset>>;

        Assets m_assets;

    private:

        RenderModule* m_renderModule;

        void initAsset(asset::Asset*);

    };

}
