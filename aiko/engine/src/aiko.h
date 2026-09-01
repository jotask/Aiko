#pragma once

#include "aiko_types.h"
#include "aiko_config.h"
#include "display/display_events.hpp"
#include "modules/module.h"
#include "systems/sytem.h"

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
    
        void close();

        const AikoConfig getConfig() const { return cfg; }

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

}
