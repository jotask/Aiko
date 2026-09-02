#pragma once

#include <aiko_types.h>
#include <layers/layer_stack.h>

#include "aiko.h"

namespace aiko
{

    class SystemConnector;
    class SystemRegistry;
    class LayerContext;

    class Application
    {
    public:
        Application();
        Application(AikoConfig cfg);
        virtual ~Application() = default;

        void pushLayer(AikoUPtr<Layer>);
        void pushOverlay(AikoUPtr<Layer>);

        void run();

    private:
        friend class Aiko;

        void connect(SystemConnector*, LayerContext*);
        void registerSystems(SystemRegistry&);

        virtual void init();
        virtual void update();
        virtual void render();
        virtual void dispose();

        void onEvent(Event& e);

    private:

        AikoUPtr<Aiko> m_aiko;
        LayerStack m_layers;

    };

}
