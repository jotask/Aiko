#include "modules/scene_module.h"

#include "components/component_entity.h"

namespace aiko
{

    void SceneModule::init()
    {

    }

    void SceneModule::update()
    {

    }

    void SceneModule::postUpdate()
    {
        auto view = m_registry.view<IUpdate>();
        for (auto [entity, cmp] : view.each())
        {
            cmp.update();
        }
    }

    void SceneModule::preRender()
    {

    }

    void SceneModule::render()
    {

    }

    void SceneModule::postRender()
    {

        auto view = m_registry.view<IRender>();

        for (auto [entity, cmp] : view.each())
        {
            cmp.render();
        }

    }

    void SceneModule::createEntity()
    {
        entt::entity e = m_registry.create();
    }

}
