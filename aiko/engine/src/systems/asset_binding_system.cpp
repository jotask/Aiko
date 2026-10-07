#include "asset_binding_system.h"

#include "scene/scene.h"
#include "systems/asset_system.h"
#include "systems/scene_system.h"
#include "systems/system_connector.h"
#include "assets/asset_binding.h"
#include "models/component.h"

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
        for (IAssetBinding* binding :
             scene.components<IAssetBinding>())
        {
            if (binding == nullptr)
            {
                continue;
            }

            if (Component* component =
                    dynamic_cast<Component*>(binding))
            {
                if (component->isActiveAndEnabled() == false)
                {
                    continue;
                }
            }

            binding->resolveAssetBinding(context);
        }
    }

}
