#pragma once

#include <memory>
#include <vector>


#include "aiko_types.h"
#include "aiko_config.h" 
#include "modules/module.h"
#include "systems/sytem.h"
#include "events/event.hpp"

namespace aiko
{

    class Application;
    class GameObject;

    class Aiko
    {
    public:

        friend class Application;

        using Modules = std::vector<AikoUPtr<Module>>;
        using Systems = std::vector<AikoUPtr<System>>;

        Aiko(Application* app);
        Aiko(Application* app, AikoConfig cfg);
        ~Aiko();

        template<class T>
        T* getSystem();
    
        void close();

        const AikoConfig getConfig() const { return cfg; }

    private:

        Application* m_application;

        void run();

        void onWindowClose(Event&);

        void init();
        void update();
        void render();
        void dispose();

        bool m_shouldStop;

         Modules m_modules;
         Systems m_systems;

        const AikoConfig cfg;

    };

    template<class T>
    inline T* Aiko::getSystem()
    {
        // Check first if system exist
        {
            auto it = std::find_if(m_systems.begin(), m_systems.end(), [](const aiko::AikoUPtr<System>& system) {
                return dynamic_cast<T*>(system.get()) != nullptr;
                });
            bool hasSystem = it != m_systems.end();
            if (hasSystem == false)
            {
                throw std::exception();
            }
        }
        auto found = std::find_if(m_systems.begin(), m_systems.end(), [](const aiko::AikoUPtr<System>& system) {
            return dynamic_cast<T*>(system.get()) != nullptr;
            });
        return (found != m_systems.end()) ? dynamic_cast<T*>(found->get()) : nullptr;
    }

}