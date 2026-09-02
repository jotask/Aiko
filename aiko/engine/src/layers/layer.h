#pragma once

#include <aiko_types.h>
#include <events/event.hpp>

namespace aiko
{
    class RenderContext;

    class Application;
    class GameObject;
    class LayerContext;
    class SystemConnector;
    class SystemRegistry;

    class InputContext;

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

        LayerContext& context();
        const LayerContext& context() const;

        GameObject* Instantiate(string name);
        GameObject* Instantiate(GameObject* parent, string name);

        float getDeltaTime() const;

        void drawRectangle(const vec3& position, const vec3& size);
        void drawMesh(const Transform& transform, const Mesh& mesh, const Material& material);

        InputContext& input();
        const InputContext& input() const;

        RenderContext& renderer();
        const RenderContext& renderer() const;

    private:
        friend class Application;

        LayerContext* m_context = nullptr;

    };
}
