#include "entity_component_system.h"

#include "modules/module_connector.h"
#include "modules/scene_module.h"

#include "components/transform_component.h"

#include "components/component_renderer.h"

namespace aiko
{
    
    aiko::AikoPtr<GameObject> EntityComponentSystem::createGameObject(std::string name)
    {
        m_gameObjects.emplace_back(std::make_shared<GameObject>());
        aiko::AikoPtr<GameObject> obj = m_gameObjects.back();
        obj->setName(name);
        obj->aiko = aiko;
        auto trans = obj->addComponent<Transform>();
        return obj;
    }
    
    void EntityComponentSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        m_sceneModule = moduleConnector->find<SceneModule>();
    }
    
    void EntityComponentSystem::init()
    {
    }
    
    void EntityComponentSystem::update()
    {
        for (auto& go : m_gameObjects) go->update();
    }
    
    void EntityComponentSystem::render()
    {
    
        for (auto& go : m_gameObjects) go->render();
    
        /*
        ImGui::Begin("ECS");
        for (auto& go : m_gameObjects)
        {
            ImGui::SetNextItemOpen(true);
            if (ImGui::TreeNode(go->getName().c_str()) == true )
            {
                for (auto& cmp : go->m_components)
                {
                    ImGui::SetNextItemOpen(true);
                    if (ImGui::TreeNode(cmp->m_name.c_str()) == true)
                    {
                        ComponentRenderer::render(cmp.get());
                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }
        }
        ImGui::End();
        */
    }
    

}
