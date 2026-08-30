#pragma once

#include "aiko_includes.h"
#include "application/application.h"

namespace sb
{

    class Sandbox : public aiko::Layer
    {
    protected:
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;
    private:

        aiko::GameObject* m_go1 = nullptr;
        aiko::GameObject* m_go2 = nullptr;
        aiko::GameObject* m_texture = nullptr;
        aiko::GameObject* m_texturePbo = nullptr;

        struct LightInst
        {
            aiko::GameObject* obj = nullptr;
            aiko::LightComponent* cmp = nullptr;
            float angle = 0.0f;
        };
        std::vector<LightInst> m_lights;

        aiko::ComputeShaderComponent* m_computeReadback = nullptr;
        bool m_computeReadbackPrinted = false;

        void initCompute();
        void initMeshes();
        void initComponents();
        void initLights();
        void initParticles();

        void updateComponents();
        void updateLights();

        void renderPrimitives();
        void renderLights();

        void updateCompute();

    };

}

