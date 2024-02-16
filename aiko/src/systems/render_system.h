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
            const static int VELOCITY = 1;
            vec2 position;
            vec2 velocity;
            float speed;

            void checkBounds(vec2 bounds);
        };

        friend class Mesh;
        friend class MeshComponent;
    
        RenderSystem();
        virtual ~RenderSystem() = default;
    
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;
    
        void updatSeeds();

        aiko::AikoPtr<Mesh> createMesh();
        aiko::AikoPtr<Light> createLight();
    
        void add(Light*);
        void render(MeshComponent*);
    
    protected:
    
        virtual void connect(ModuleConnector*, SystemConnector*) override;
    
    private:
    
        const int nMaxParticles = 100;

        int radiusLoc;
        int resolutionLoc;
        int positionLoc;
        int nParticlesLoc;

        int iResolutionLoc;
        int iTimeLoc;
        int iTimeDeltaLoc;
        int iFrameRateLoc;
        int iFrameLoc;
        int iChannelTimeLoc;
        int iChannelResolutionLoc;
        int iMouseLoc;
        int iDateLoc;

        vec3      iResolution;           // viewport resolution (in pixels)
        float     iTime;                 // shader playback time (in seconds)
        float     iTimeDelta;            // render time (in seconds)
        float     iFrameRate;            // shader frame rate
        int       iFrame;                // shader playback frame
        float     iChannelTime[4];       // channel playback time (in seconds)
        vec3      iChannelResolution[4]; // channel resolution (in pixels)
        vec4      iMouse;                // mouse pixel coords. xy: current (if MLB down), zw: click
        vec4      iDate;                 // (year, month, day, time in seconds)

        float radius = 1.0f;
        int nParticles = nMaxParticles;
        float all_velocity = 1.0f;

        Shader m_shader;


        void RegenerateSeeds();

        int currentShader = 0;
        void NextShader(bool first = false);
    
        aiko::AikoPtr<RenderModule> m_renderModule;
        std::vector<Particle> m_particles;
        RenderComponentTexture* m_rendererComponentTexture;
    
    };

}
