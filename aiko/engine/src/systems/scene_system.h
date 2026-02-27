#pragma once

#include "systems/base_system.h"
#include "types/scene_types.h"

#include "scene/scene_render.h"
#include "scene/scene_view_builder.h"

namespace aiko
{

    class EntityComponentSystem;
    class RenderModule;
    
    class SceneSystem : public BaseSystem
    {
    public:
        SceneSystem();
        virtual ~SceneSystem() = default;
    protected:
    
        virtual void connect(ModuleConnector*, SystemConnector*) override;
        virtual void render() override;
    private:

        EntityComponentSystem* m_entitySystem;
        RenderModule* m_renderModule;

        SceneRenderer m_sceneRenderer;
        SceneViewBuilder m_sceneViewBuilder;

    };

}
