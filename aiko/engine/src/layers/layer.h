#pragma once

#include <aiko_types.h>
#include <math/math.h>
#include <input/inputs_types.h>
#include <events/event.hpp>

namespace aiko
{

    class Application;
    class GameObject;
    class LayerContext;
    class SystemConnector;
    class SystemRegistry;

    class Layer
    {
    public:
        Layer() = default;
        virtual ~Layer() = default;

        virtual void onAttach() {}
        virtual void onDetach() {}

        virtual void registerSystems(SystemRegistry&) {}
        virtual void connect(SystemConnector*) {}

        virtual void init() {}
        virtual void dispose() {}

        virtual void update() {}
        virtual void render() {}
        virtual void onEvent(Event& e) {}

    protected:
        GameObject* Instantiate(string name);
        GameObject* Instantiate(GameObject* parent, string name);

        bool isKeyPressed(Key key) const;
        bool isKeyJustPressed(Key key) const;

        vec2 getMousePosition() const;
        bool isMouseButtonPressed(MouseButton button) const;

        float getDeltaTime() const;

    private:
        friend class Application;

        LayerContext* m_context = nullptr;

    };
}
