#pragma once

#include <memory>
#include <vector>

#include "aiko_types.h"
#include "systems/base_system.h"
#include "models/game_object.h"
#include "models/camera.h"
#include "models/shader.h"
#include "types/camera_types.h"
#include "shared/math.h"
#include "types/asset_type.h"

namespace aiko
{

    class AssetModule;
    class RenderModule;
    
    class AssetSystem : public BaseSystem
    {
    public:
    
        using CameraPtr = std::shared_ptr<Camera>;

        AssetSystem() = default;
        virtual ~AssetSystem() = default;
    
        asset::Shader* loadShader(const char*, const char*);

        void unload(asset::Asset&);
        void unload(asset::ID);

    protected:
    
        virtual void connect(ModuleConnector*, SystemConnector*) override;
    
        virtual void init() override;
    
        virtual void update() override;
    
        virtual void render() override;

    private:

        AssetModule* m_assetModule;
        RenderModule* m_renderModule;

    };

}
