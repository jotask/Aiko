#pragma once

#include "systems/base_system.h"

#include "scene/scene_view_builder.h"

#include "models/game_object.h"
#include "scene/scene.h"

#include <models/camera.h>

namespace aiko
{

    class RenderModule;
    class RenderSystem;
    class GameObject;
    
    class SceneSystem : public BaseSystem
    {
    public:
        SceneSystem();
        virtual ~SceneSystem() = default;

        AikoPtr<GameObject> createGameObject(string name = DEFAULT_OBJECT_NAME);
        AikoPtr<GameObject> createGameObject(GameObject* parent, string name = DEFAULT_OBJECT_NAME);
        void destroyGameObject(const AikoPtr<GameObject>& obj);

        Camera* getMainCamera();
        const Camera* getMainCamera() const;

        void setActiveCamera(GameObject* obj);
        GameObject* getActiveCamera() { return m_scene.getActiveCamera(); }
        const GameObject* getActiveCamera() const { return m_scene.getActiveCamera(); }

        Scene& getScene();
        const Scene& getScene() const;

    protected:

        virtual void connect(ModuleConnector*, SystemConnector*) override;
        virtual void render() override;

        constexpr static const char* DEFAULT_OBJECT_NAME = "Game Object";

    private:

        RenderModule* m_renderModule;
        RenderSystem* m_renderSystem;

        SceneViewBuilder m_sceneViewBuilder;
        Scene m_scene;

    };

}
