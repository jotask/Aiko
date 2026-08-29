#pragma once

#include "assets/asset_manager.h"
#include "modules/base_module.h"

#include "assets/assets.h"

namespace aiko
{

    class AssetsManagerModule : public BaseModule
    {
    
    public:

        AssetsManagerModule(Aiko* aiko);
        virtual ~AssetsManagerModule() = default;

        AssetManager* getManager() { return &m_manager; }
        Assets& getAssets() { return m_assets; }

    protected:

        virtual void preInit() override;
        virtual void init() override;
        virtual void update() override;
        virtual void beginFrame() override;
        virtual void endFrame() override;
        virtual void dispose() override;

    private:

        AssetManager m_manager;
        Assets m_assets;

    };

}
