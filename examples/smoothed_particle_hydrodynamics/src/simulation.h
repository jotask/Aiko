#pragma once

#include "aiko_types.h"
#include "particle_emitter.h"
#include "shape.h"
#include "sph_types.h"

namespace sph
{

    class Simulation
    {
    public:

        void init(size_t particleCount);

        void updateEmitters( float dt,aiko::vector<SPHParticle>& spawnedParticles);

        const aiko::vector<SPHParticle>& initialParticles() const { return m_initialParticles; }

        const aiko::vector<ParticleEmitter>& emitters() const { return m_emitters; }
        aiko::vector<ParticleEmitter>& emitters() { return m_emitters; }

        const SPHParameters& parameters() const { return m_parameters; }
        SPHParameters& parameters() { return m_parameters; }

        const WorldBounds& bounds() const { return m_bounds; }

        const aiko::vector<Shape>& shapes() const { return m_shapes; }
        aiko::vector<Shape>& shapes() { return m_shapes; }

        ParticleEmitter* createParticleEmitter(EmitterSettings settings);

        void removeParticleEmitter(size_t index);

        Shape* createShape(aiko::vec3 position, aiko::MeshAsset asset, aiko::Color color);

        void removeShape(size_t index);

    private:

        aiko::vector<SPHParticle> m_initialParticles;

        const WorldBounds m_bounds
        {
            .position = { 0.0f, 0.0f, 0.0f},
            .size = { 32.0f, 32.0f, 0.0f}
        };

        SPHParameters m_parameters;

        aiko::vector<ParticleEmitter> m_emitters;
        aiko::vector<Shape> m_shapes;
    };

}
