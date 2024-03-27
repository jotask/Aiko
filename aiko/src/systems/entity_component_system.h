#pragma once

#include <memory>
#include <vector>
#include <string>

#include "aiko_types.h"
#include "systems/base_system.h"
// #include "models/game_object.h"
#include "types/scene_types.h"

namespace aiko
{
    
    class SceneModule;
    class GameObject;
    
    class EntityComponentSystem : public BaseSystem
    {
        friend class GameObject;
    public:
    
        EntityComponentSystem() = default;
        virtual ~EntityComponentSystem() = default;
    
        aiko::AikoPtr<GameObject> createGameObject(std::string name = "Game Object");
    
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
    
        SceneModule* m_sceneModule;
    
        std::vector<aiko::AikoPtr<GameObject>> m_gameObjects;

    };

}
