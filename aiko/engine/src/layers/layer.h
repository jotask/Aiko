#pragma once

#include <events/event.hpp>

namespace aiko
{

    class Application;
    class SystemConnector;

    class Layer
    {
    public:
        Layer() = default;
        virtual ~Layer() = default;

        virtual void onAttach() {}
        virtual void onDetach() {}

        virtual void registerSystems() {}
        virtual void connect(SystemConnector*) {}

        virtual void init() {}
        virtual void dispose() {}

        virtual void update() {}
        virtual void render() {}
        virtual void onEvent(Event& e) {}

    protected:

        friend class Application;
        Application* app;

    };
}