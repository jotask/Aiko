#pragma once

#include "aiko_includes.h"
#include "layers/layer.h"
#include "models/material.h"
#include "models/mesh.h"
#include "models/texture.h"
#include "models/compute_buffer.h"
#include "models/font.h"
#include "models/render_target.h"
#include "models/camera.h"

#include <array>

namespace aiko
{
    class ComputeShaderComponent;
    class GameObject;
    class LightComponent;
    class RenderSystem;
    class SystemConnector;
    class SpriteComponent;
}

namespace aiko::lab
{
    class VisualLab final : public Layer
    {
    public:
        VisualLab() = default;
        ~VisualLab() override = default;

    protected:
        void init() override;
        void update() override;
        void render() override;
        void dispose() override;
        void connect(SystemConnector& systemConnector) override;

    };
}
