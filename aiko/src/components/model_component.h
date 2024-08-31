#pragma once

#include "aiko_types.h"
#include "models/component.h"
#include "models/model.h"

namespace aiko
{

    class RenderSystem;

    class ModelComponent : public Component, public IUpdate, public IRender3D
    {
    public:

        friend class RenderSystem;

        ModelComponent();
        virtual ~ModelComponent() = default;

        virtual void init() override;
        virtual void update() override;
        virtual void render() override;

    private:
        RenderSystem*   m_renderSystem;
        Model m_model;
    };

}