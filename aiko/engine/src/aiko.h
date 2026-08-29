#pragma once

#include <memory>
#include <algorithm>

#include "aiko_types.h"
#include "aiko_config.h"
#include "display/display_events.hpp"
#include "modules/module.h"
#include "systems/sytem.h"
#include "events/event.hpp"

namespace aiko
{
    class AssetManager;

    class Application;
    class GameObject;

    class Aiko
    {
    public:

        friend class Application;

        using Modules = vector<AikoUPtr<Module>>;
        using Systems = vector<AikoUPtr<System>>;

        Aiko(Application* app);
        Aiko(Application* app, AikoConfig cfg);
        ~Aiko();

        template<class T>
        T* getSystem();
    
        void close();

        const AikoConfig getConfig() const { return cfg; }

        template<typename TSystem, typename... Args>
        TSystem* registerSystem(Args&& ...);

    private:

        Application* m_application;

        void run();

        void onWindowClose(WindowCloseEvent&);

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

    template <typename TSystem, typename ... Args>
    TSystem* Aiko::registerSystem(Args&&... args)
    {
        static_assert(std::is_base_of_v<System, TSystem>, "System must derive from System");
        auto instance = std::make_unique<TSystem>(std::forward<Args>(args)...);
        instance->aiko = this;
        TSystem* raw = instance.get();
        m_systems.emplace_back(std::move(instance));
        return raw;
    }
}
