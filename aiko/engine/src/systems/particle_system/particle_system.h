#pragma once

#include <vector>

#include "aiko_types.h"
#include "systems/base_system.h"
#include "systems/particle_system/particle_emitter.h"

namespace aiko
{
    class RenderModule;
    class ParticleSystem : public BaseSystem
    {
    public:

        ParticleSystem() = default;
        virtual ~ParticleSystem() = default;

        ParticleEmitter* createEmitter(size_t nParticle);

    protected:
    
        virtual void connect(ModuleConnector*, SystemConnector*) override;
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;
    
    private:
        RenderModule* m_renderModule;

        std::vector<ParticleEmitter> m_emitters;

    };

}
