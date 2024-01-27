#pragma once

#include <memory>
#include <vector>
#include <string>

#include "aiko_types.h"
#include "systems/base_system.h"
#include "models/game_object.h"
#include "models/mesh.h"
#include "models/light.h"
#include "modules/renderer/render_module.h"

namespace aiko
{
    
    class SceneModule;
    class RenderComponentTexture;
    
    class RenderSystem : public BaseSystem
    {
    public:
    
        class Particle
        {
        public:
            vec2 position;
            vec2 velocity;

            void checkBounds(vec2 bounds);
        };

        friend class Mesh;
        friend class MeshComponent;
    
        RenderSystem();
        virtual ~RenderSystem() = default;
    
        virtual void init() override;
        virtual void update() override;
    
        void updatSeeds();

        aiko::AikoPtr<Mesh> createMesh();
        aiko::AikoPtr<Light> createLight();
    
        void add(Light*);
        void render(MeshComponent*);
    
    protected:
    
        virtual void connect(ModuleConnector*, SystemConnector*) override;
    
    private:
    
        const int nParticles = 100;

        int positionLoc;

        RenderTexture2D m_target;
        Shader m_shader;

        void RegenerateSeeds();
    
        aiko::AikoPtr<RenderModule> m_renderModule;
        std::vector<Particle> m_seeds;
        RenderComponentTexture* m_rendererComponentTexture;
    
    };

}
