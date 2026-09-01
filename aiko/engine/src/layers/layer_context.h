#pragma once

#include <aiko_types.h>
#include <math/math.h>
#include <input/inputs_types.h>

namespace aiko
{
    class GameObject;
    class InputSystem;
    class SceneSystem;
    class SystemConnector;

    class LayerContext
    {
    public:
        explicit LayerContext(SystemConnector&);

        GameObject* Instantiate(string name);
        GameObject* Instantiate(GameObject* parent, string name);

        bool isKeyPressed(Key key) const;
        bool isKeyJustPressed(Key key) const;

        vec2 getMousePosition() const;
        bool isMouseButtonPressed(MouseButton button) const;

        float getDeltaTime() const;

    private:
        InputSystem* m_inputSystem = nullptr;
        SceneSystem* m_sceneSystem = nullptr;
    };
}
