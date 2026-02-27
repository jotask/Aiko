#pragma once

#include "aiko_types.h"
#include "models/component.h"
#include "models/mesh.h"
#include "models/material.h"

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

        void load(const char*);
        void refresh();

        RenderSystem*   m_renderSystem;
        Mesh   m_mesh;
        Material m_material;
    };

}