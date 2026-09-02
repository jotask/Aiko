#pragma once

#include <math/math.h>

namespace aiko
{

    class SystemConnector;
    class RenderSystem;
    class Mesh;
    class Material;
    struct Transform;

    class RenderContext
    {
    public:
        void drawRectangle(const vec3& position, const vec3& size);
        void drawMesh(const Transform& transform, const Mesh& mesh, const Material& material);

    private:
        friend class LayerContext;

        explicit RenderContext(SystemConnector& connector);

        RenderSystem* m_renderSystem = nullptr;
    };
}
