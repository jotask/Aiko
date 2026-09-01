#pragma once

#include <aiko_types.h>
#include <math/math.h>
#include <layers/layer_stack.h>
#include <input/inputs_types.h>

#include "aiko.h"

namespace aiko
{

    class InputSystem;
    class SceneSystem;
    class SystemConnector;
    class SystemRegistry;

    class Application
    {
    public:
        Application();
        Application(AikoConfig cfg);
        virtual ~Application() = default;

        void pushLayer(AikoUPtr<Layer>);
        void pushOverlay(AikoUPtr<Layer>);

        void connect(SystemConnector*);
        void registerSystems(SystemRegistry&);

        void run();

    protected:

        float getlDeltaTime() const;
        bool isKeyPressed(Key) const;
        bool isKeyJustPressed(Key) const;
        vec2 getMousePosition() const;
        bool isMouseButtonPressed(MouseButton) const;

        virtual void init();
        virtual void update();
        virtual void render();
        virtual void dispose();

        GameObject* Instantiate(string name);
        GameObject* Instantiate(GameObject* , string name);

        void setActiveCamera(GameObject* obj);

        void onEvent(Event& e);

    private:

        AikoUPtr<Aiko> m_aiko;
        LayerStack m_layers;

        InputSystem* m_inputSystem = nullptr;
        SceneSystem* m_sceneSystem = nullptr;

    };

}
