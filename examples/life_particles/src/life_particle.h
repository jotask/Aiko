#pragma once

#include "particles_type.h"
#include "particle.h"

#include <application/application.h>

#include <array>

#include "models/material.h"
#include "models/mesh_factory.h"

namespace lp
{

    class LifeParticles : public aiko::Layer
    {
    protected:
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;
        virtual void dispose() override;
    private:

        std::array<Particle, c_particles_amount> m_particles;

        aiko::Mesh m_mesh;
        aiko::Material m_material;
    
    };

}

