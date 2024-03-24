#pragma once

#include "models/component.h"
#include "types/camera_types.h"

#include "particle.h"

#include <vector>

namespace sandbox
{

    class ParticlesComponent : public aiko::Component
    {
    public:
        ParticlesComponent();
        virtual ~ParticlesComponent() = default;
        virtual void update() override;
        virtual void render() override;
    protected:
        virtual void init() override;
    private:
        std::vector<Particle> m_particles;
    };

}
