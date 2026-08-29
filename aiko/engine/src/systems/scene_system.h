#pragma once

#include "systems/base_system.h"
#include "types/scene_types.h"

#include "scene/scene_view_builder.h"

#include "models/game_object.h"
#include "scene/scene.h"
#include "../models/component_context.h"

#include <models/camera.h>

namespace aiko
{

    class RenderModule;
    class RenderSystem;
    class GameObject;
    class AssetSystem;
    
    class SceneSystem : public BaseSystem
    {
        friend class GameObject;
    public:
        SceneSystem();
        virtual ~SceneSystem() = default;

        AikoPtr<GameObject> createGameObject(string name = DEFAULT_OBJECT_NAME);
        AikoPtr<GameObject> createGameObject(GameObject* parent, string name = DEFAULT_OBJECT_NAME);
        void destroyGameObject(const AikoPtr<GameObject>& obj);

        Camera* getMainCamera();
        const Camera* getMainCamera() const;

        void setActiveCamera(GameObject* obj);
        GameObject* getActiveCamera() const { return m_scene.getActiveCamera(); }

        Scene& getScene();
        const Scene& getScene() const;

    protected:

        virtual void connect(ModuleConnector*, SystemConnector*) override;
        virtual void update() override;
        virtual void render() override;

        constexpr static const char* DEFAULT_OBJECT_NAME = "Game Object";

    private:

        RenderModule* m_renderModule;
        RenderSystem* m_renderSystem;
        AssetSystem* m_assetSystem;

        ComponentContext m_componentContext;

        SceneViewBuilder m_sceneViewBuilder;
        Scene m_scene;

    };

}
