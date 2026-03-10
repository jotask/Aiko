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
        aiko::GameObject* m_go1;
        aiko::GameObject* m_go2;
        aiko::GameObject* m_texture;
        aiko::GameObject* m_texturePbo;

        struct LightInst
        {
            aiko::GameObject* obj;
            aiko::AikoPtr<aiko::LightComponent> cmp;
            float angle;
        };
        std::vector<LightInst> m_lights;

    };

}

