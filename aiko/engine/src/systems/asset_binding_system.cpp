#include "asset_binding_system.h"

#include "scene/scene.h"
#include "systems/asset_system.h"
#include "systems/scene_system.h"
#include "systems/system_connector.h"
#include "assets/asset_binding.h"

namespace aiko
{
    void AssetBindingSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_SYSTEM_REQUIRED(AssetSystem, systemConnector, m_assetSystem);
        BIND_SYSTEM_REQUIRED(SceneSystem, systemConnector, m_sceneSystem);
    }

    void AssetBindingSystem::update()
    {
        BaseSystem::update();

        AssetBindingContext context(*m_assetSystem);

        Scene& scene = m_sceneSystem->getScene();

        for (GameObject* object : scene.consumeAssetBindingDirtyObjects())
        {
            if (object == nullptr)
            {
                continue;
            }

            for (Component* component : object->getComponents())
            {
                if (auto* binding = dynamic_cast<IAssetBinding*>(component))
                {
                    if (binding->resolveAssetBinding(context))
                    {
                        scene.markAssetBindingDirty(*object);
                    }
                }
            }
        }
    }

}
