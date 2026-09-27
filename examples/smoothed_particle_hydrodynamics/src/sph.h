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

#include "playground.h"

namespace aiko
{
    class SystemConnector;
    class RenderSystem;
};

namespace sph
{
    class SPHFluidSimulation final : public aiko::Layer
    {
    public:
        SPHFluidSimulation() = default;
        ~SPHFluidSimulation() override = default;

    protected:
        void init() override;
        void update() override;
        void render() override;
        void connect(aiko::SystemConnector& systemConnector) override;

    private:
        aiko::RenderSystem* m_renderSystem = nullptr;
        aiko::CameraComponent* m_cameraComponent = nullptr;
        Playground m_playground;
    };
}
