#pragma once

#include "assets/asset_manager.h"
#include "modules/base_module.h"

namespace aiko
{

    class AssetsManagerModule : public BaseModule
    {
    
    public:

        AssetsManagerModule(Aiko* aiko);
        virtual ~AssetsManagerModule() = default;

        AssetManager* getManager() { return &m_manager; }

    protected:

        virtual void init() override;
        virtual void update() override;
        virtual void beginFrame() override;
        virtual void endFrame() override;
        virtual void dispose() override;

    private:

        AssetManager m_manager;

    };

}
