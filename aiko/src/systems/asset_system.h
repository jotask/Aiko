#pragma once

#include <memory>
#include <vector>

#include "aiko_types.h"
#include "systems/base_system.h"
#include "models/game_object.h"
#include "models/camera.h"
#include "models/shader.h"
#include "models/mesh.h"
#include "models/model.h"
#include "types/camera_types.h"
#include "shared/math.h"
#include "types/asset_type.h"
#include "core/uuid.h"

#include "assets/asset.h"

namespace aiko
{

    class RenderSystem;
    
    class AssetSystem : public BaseSystem
    {
    public:
    
        using CameraPtr = std::shared_ptr<Camera>;

        AssetSystem() = default;
        virtual ~AssetSystem() = default;

        template<class T>
        T* load();

        void unload(asset::Asset*);
        void unload(asset::ID);

        RenderSystem* getRenderSystem() const { return m_renderSystem; }

        Mesh loadMesh(const char*);
        Model loadModel(const char*);

    protected:
    
        virtual void connect(ModuleConnector*, SystemConnector*) override;
    
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;

    private:

        using Assets = std::vector<AikoPtr<asset::Asset>>;
        Assets m_assets;

        RenderSystem* m_renderSystem;

    };

    template<class T>
    inline T* AssetSystem::load()
    {
        static_assert(std::is_base_of<BaseAsset, T>::value, "T must be derived from Asset");
        AikoPtr<asset::Asset> ass = std::make_shared<T>();
        m_assets.push_back(ass);
        ass->connect(nullptr);
        return nullptr;
    }

}
