#pragma once

#include <memory>
#include <vector>

#include "aiko_types.h"
#include "systems/base_system.h"
#include "models/game_object.h"
#include "models/camera.h"
#include "types/camera_types.h"
#include "shared/math.h"

namespace aiko
{

    class AssetModule;
    
    class AssetSystem : public BaseSystem
    {
    public:
    
        using CameraPtr = std::shared_ptr<Camera>;

        AssetSystem() = default;
        virtual ~AssetSystem() = default;
    
    protected:
    
        virtual void connect(ModuleConnector*, SystemConnector*) override;
    
        virtual void init() override;
    
        virtual void update() override;
    
        virtual void render() override;
    
    private:

        AssetModule* m_assetModule;

    };

}
