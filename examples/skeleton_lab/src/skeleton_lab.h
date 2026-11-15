#pragma once

#include <aiko_includes.h>
#include <models/model.h>
#include <layers/layer.h>

namespace lab
{

    class SkeletonLab : public aiko::Layer
    {
    protected:
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;
    private:

        aiko::Model m_model;
        float m_animationTime = 0.0f;

    };

}

