#pragma once

#include "models/component.h"
#include "models/mesh.h"

namespace aiko
{

    class RenderSystem;

    class MeshComponent : public Component
    {
    public:
        MeshComponent();
        virtual ~MeshComponent() = default;

        virtual void init() override;
        virtual void update() override;
        virtual void render() override;

    private:
        RenderSystem* m_renderSystem;
        aiko::AikoPtr<Mesh> m_mesh;

    };

}