#pragma once

#include "systems/base_system.h"

namespace aiko
{
    class AssetSystem;
    class SceneSystem;

    class AssetBindingSystem : public BaseSystem
    {
    public:
        AssetBindingSystem() = default;
        virtual ~AssetBindingSystem() override = default;

    protected:
        virtual void connect(ModuleConnector*, SystemConnector*) override;
        virtual void update() override;

    private:

        AssetSystem* m_assetSystem = nullptr;
        SceneSystem* m_sceneSystem = nullptr;
    };
}
