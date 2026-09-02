#pragma once

#include <aiko_types.h>
#include <math/math.h>
#include <input/inputs_types.h>

namespace aiko
{
    class GameObject;
    class InputSystem;
    class SceneSystem;
    class RenderSystem;
    class AssetSystem;
    class SystemConnector;
    class Mesh;
    class Material;

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
        vec2 getMouseDelta() const;
        void setIsMouseCentred(bool centred) const;

        float getDeltaTime() const;

        // Draw
        void drawRectangle(const vec3& position, const vec3& size);
        void drawMesh(const Transform& transform, const Mesh& mesh, const Material& material);

        AssetId loadShader(string_view source);

    private:
        InputSystem* m_inputSystem = nullptr;
        SceneSystem* m_sceneSystem = nullptr;
        RenderSystem* m_renderSystem = nullptr;
        AssetSystem* m_assetSystem = nullptr;
    };
}
