#pragma once

#include <memory>
#include <vector>
#include <string>

#include "aiko_types.h"
#include "aiko_config.h" 
#include "modules/module.h"
#include "systems/sytem.h"

class DisplayModule;
class Application;
class GameObject;

class Aiko
{
public:

    friend class Application;

    Aiko(Application* app);
    Aiko(Application* app, AikoConfig cfg);
    ~Aiko();

    template<class T>
    aiko::AikoPtr<T> getSystem();

    GameObject* createGameObject(std::string name = "Game Object");
    

private:

    Application* m_application;

    void run();

    void init();
    void update();
    void render();
    void dispose();

    std::vector<std::shared_ptr<Module>> m_modules;
    std::vector<std::shared_ptr<System>> m_systems;

    DisplayModule* m_displayModule;
    AikoConfig cfg;

};

template<class T>
inline aiko::AikoPtr<T> Aiko::getSystem()
{
    // Check first if system exist
    {
        auto it = std::find_if(m_systems.begin(), m_systems.end(), [](const aiko::AikoPtr<System>& system) {
            return dynamic_cast<T*>(system.get()) != nullptr;
            });
        bool hasSystem = it != m_systems.end();
        if (hasSystem == false)
        {
            throw std::exception();
        }
    }
    auto found = std::find_if(m_systems.begin(), m_systems.end(), [](const std::shared_ptr<System>& system) {
        return dynamic_cast<T*>(system.get()) != nullptr;
        });
    return (found != m_systems.end()) ? std::dynamic_pointer_cast<T>(*found) : nullptr;
}
