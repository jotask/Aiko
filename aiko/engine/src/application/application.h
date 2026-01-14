#pragma once

#include <aiko_types.h>
#include <math/math.h>

#include <input/inputs_types.h>

#include "aiko.h"

namespace aiko
{

    class Shader;
    class Camera;
    class RenderSystem;
    class InputSystem;
    class AssetSystem;

    class Application
    {
    public:

        friend class Aiko;

        Application();
        Application(AikoConfig cfg);
        virtual ~Application() = default;

        void run();

    protected:

        float getlDeltaTime() const;
        bool isKeyPressed(Key) const;
        bool isKeyJustPressed(Key) const;
        vec2 getMousePosition() const;
        bool isMouseButtonPressed(MouseButton) const;

        virtual void init() {}
        virtual void update() {}
        virtual void render() {}

        InputSystem* getInputSystem() const;
        RenderSystem* getRenderSystem() const;

        Camera* getMainCamera();
        vec2 getViewportSize() const;
        float getAspectRatio() const;

        aiko::AikoUPtr<Aiko> m_aiko;

        GameObject* Instantiate(string name);
        GameObject* Instantiate(GameObject* , string name);

        // Draw text
        void drawText(string, float, float);

    private:

    };

}
