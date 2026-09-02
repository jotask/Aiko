#pragma once

#include <aiko_types.h>
#include <aiko_config.h>

namespace aiko
{

    class Aiko;
    class Event;
    class Layer;
    class LayerStack;
    class SystemConnector;
    class SystemRegistry;
    class LayerContext;

    class Application
    {
    public:
        Application();
        Application(AikoConfig cfg);
        virtual ~Application();

        void pushLayer(AikoUPtr<Layer>);
        void pushOverlay(AikoUPtr<Layer>);

        void run();

    protected:
        virtual void init();
        virtual void update();
        virtual void render();
        virtual void dispose();

    private:
        friend class Aiko;

        AikoUPtr<Aiko> m_aiko;
        AikoUPtr<LayerStack> m_layers;

        void connect(SystemConnector&, LayerContext&);
        void registerSystems(SystemRegistry&);

        void onEvent(Event& e);

    };

}
