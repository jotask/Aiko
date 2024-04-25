#pragma once

#include "aiko_types.h"
#include "models/component.h"
#include "models/mesh.h"
#include "models/shader.h"

namespace aiko
{

    class RenderSystem;

    class MeshComponent : public Component, public IUpdate, public IRender3D
    {
    public:

        friend class RenderSystem;

        MeshComponent();
        virtual ~MeshComponent() = default;

        virtual void init() override;
        virtual void update() override;
        virtual void render() override;

    private:
        RenderSystem*   m_renderSystem;
        AikoPtr<Mesh>   m_mesh;
        AikoPtr<Shader> m_shader;

    };

}