#pragma once

#include <aiko_types.h>
#include <math/math.h>
#include <layers/layer_stack.h>
#include <input/inputs_types.h>

#include "aiko.h"

#include <utility>

namespace aiko
{

    class Shader;
    class Camera;
    class RenderSystem;
    class InputSystem;
    class AssetSystem;
    class SystemConnector;

    class Application
    {
    public:
        Application();
        Application(AikoConfig cfg);
        virtual ~Application() = default;

        void pushLayer(AikoUPtr<Layer>);
        void pushOverlay(AikoUPtr<Layer>);

        void connect(SystemConnector*);

        void run();

    public:

        float getlDeltaTime() const;
        bool isKeyPressed(Key) const;
        bool isKeyJustPressed(Key) const;
        vec2 getMousePosition() const;
        bool isMouseButtonPressed(MouseButton) const;

        virtual void init();
        virtual void update();
        virtual void render();
        virtual void dispose();

        InputSystem* getInputSystem() const;
        RenderSystem* getRenderSystem() const;

        Camera* getMainCamera();
        vec2 getViewportSize() const;
        float getAspectRatio() const;

        void registerSystems();

        GameObject* Instantiate(string name);
        GameObject* Instantiate(GameObject* , string name);

        void setActiveCamera(GameObject* obj);

        void onEvent(Event& e);

        template<typename TSystem, typename... Args>
        TSystem* registerSystem(Args&&... args)
        {
            return m_aiko->registerSystem<TSystem>(std::forward<Args>(args)...);
        }

    private:

        AikoUPtr<Aiko> m_aiko;
        LayerStack m_layers;

    };

}
