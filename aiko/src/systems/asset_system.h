#pragma once

#include <memory>
#include <vector>
#include <map>

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

    class RenderSystem;
    
    class AssetSystem : public BaseSystem
    {
    public:
    
        using CameraPtr = std::shared_ptr<Camera>;

        AssetSystem() = default;
        virtual ~AssetSystem() = default;
    
        Shader* loadShader(const char*, const char*);

        void unload(asset::Asset&);
        void unload(asset::ID);

        RenderSystem* getRenderSystem() const { return m_renderSystem; }

    protected:
    
        virtual void connect(ModuleConnector*, SystemConnector*) override;
    
        virtual void init() override;
    
        virtual void update() override;
    
        virtual void render() override;

    private:

        void initAsset(asset::Asset*);

        using Assets = std::map<asset::ID, AikoUPtr<asset::Asset>>;
        Assets m_assets;

        RenderSystem* m_renderSystem;

    };

}
