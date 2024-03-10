#pragma once

#include <vector>

#include "application/application.h"
#include "models/shader.h"

#include "particle.h"

namespace life
{

    class LifeParticle : public aiko::Application
    {
    protected:

        virtual void init() override;
        virtual void update() override;
        virtual void render() override;

    private:

        const unsigned int MAX_NUM_TOTAL_PARTICLES = 1000;

        int iParticlesLoc;
        int iRadiusLoc;
        int iResolutionLoc;

        float particleRadius = 1.0f;

        std::vector<Particle> m_particles;

        aiko::asset::Shader* m_shader = nullptr;

    };

}

