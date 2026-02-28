#pragma once

#include "systems/base_system.h"
#include "types/scene_types.h"

#include "scene/scene_render.h"
#include "scene/scene_view_builder.h"

#include "models/game_object.h"
#include "scene/scene.h"

namespace aiko
{

    class RenderModule;
    class GameObject;
    
    class SceneSystem : public BaseSystem
    {
        friend class GameObject;
    public:
        SceneSystem();
        virtual ~SceneSystem() = default;

        // ---- Scene ownership API (temporary forward to ECS) ----
        AikoPtr<GameObject> createGameObject(string name = DEFAULT_OBJECT_NAME);
        AikoPtr<GameObject> createGameObject(GameObject* parent, string name = DEFAULT_OBJECT_NAME);
        void destroyGameObject(const AikoPtr<GameObject>& obj);

    protected:
    
        virtual void connect(ModuleConnector*, SystemConnector*) override;
        virtual void update() override;
        virtual void render() override;

        constexpr static const char* DEFAULT_OBJECT_NAME = "Game Object";

        Scene& getScene();
        const Scene& getScene() const;

    private:

        RenderModule* m_renderModule;

        SceneRenderer m_sceneRenderer;
        SceneViewBuilder m_sceneViewBuilder;

        Scene m_scene;

    };

}
