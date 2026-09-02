#include "layers/layer_context.h"

#include "systems/input_system.h"
#include "systems/scene_system.h"
#include "systems/system_connector.h"
#include "systems/asset_system.h"
#include "systems/render_system.h"

#include <time/time.h>

#include <utility>

namespace aiko
{
    LayerContext::LayerContext(SystemConnector& connector)
        : m_input(connector)
    {

        m_sceneSystem = connector.find<SceneSystem>();
        AIKO_ASSERT(m_sceneSystem != nullptr, "Required system SceneSystem not found");

        m_renderSystem = connector.find<RenderSystem>();
        AIKO_ASSERT(m_renderSystem != nullptr, "Required system RenderSystem not found");

        m_assetSystem = connector.find<AssetSystem>();
        AIKO_ASSERT(m_assetSystem != nullptr, "Required system AssetSystem not found");

    }

    GameObject* LayerContext::Instantiate(string name)
    {
        return m_sceneSystem->createGameObject(std::move(name)).get();
    }

    GameObject* LayerContext::Instantiate(GameObject* parent, string name)
    {
        return m_sceneSystem->createGameObject(parent, std::move(name)).get();
    }

    float LayerContext::getDeltaTime() const
    {
        return Time::it().getDeltaTime();
    }

    void LayerContext::drawRectangle(const vec3& position, const vec3& size)
    {
        m_renderSystem->renderRectangle(position, size);
    }

    void LayerContext::drawMesh(const Transform& transform, const Mesh& mesh, const Material& material)
    {
        m_renderSystem->render(transform, mesh, material);
    }

    AssetId LayerContext::loadShader(string_view source)
    {
        const AssetId id = m_assetSystem->registerAsset<ShaderAsset>(source);
         m_assetSystem->loadAsset<ShaderAsset>(id);
        return id;
    }

}
