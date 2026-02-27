#pragma once

#include <memory>
#include <vector>


#include "aiko_types.h"
#include "systems/base_system.h"
#include "types/scene_types.h"
#include "scene/scene.h"

#include "scene/scene_render.h"

namespace aiko
{

    class GameObject;
    
    class EntityComponentSystem : public BaseSystem
    {
        friend class GameObject;
    public:
    
        EntityComponentSystem() = default;
        virtual ~EntityComponentSystem() = default;

        constexpr static const char* DEFAULT_OBJECT_NAME = "Game Object";

        aiko::AikoPtr<GameObject> createGameObject(string name = DEFAULT_OBJECT_NAME);
        aiko::AikoPtr<GameObject> createGameObject(GameObject* parent, string name = DEFAULT_OBJECT_NAME);
        void destroyGameObject(GameObject*);
    
        std::vector<GameObject*> getObjects();

        Scene& scene() { return m_scene; }
        const Scene& scene() const { return m_scene; }

    protected:
    
        virtual void connect(ModuleConnector*, SystemConnector*) override;
    
        virtual void init() override;
    
        virtual void update() override;
    
        virtual void render() override;
    

    private:

        template<class T>
        void onComponentAdded(GameObject*, T*);

        template<class T>
        void onComponentRemoved(GameObject*, T*);

        SceneObject createEntity();

        SceneRenderer m_sceneRenderer;
        Scene m_scene;

    };

}
