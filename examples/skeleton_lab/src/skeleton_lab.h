#pragma once

#include <aiko_includes.h>
#include <models/model.h>
#include <layers/layer.h>
#include <math/math_transform.h>

#include <cmath>

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
        aiko::Model m_modelB;
        bool m_playbackStarted = false;

        aiko::GameObject* m_lightObject = nullptr;

        float m_externalPoseTime = 0.0f;
        aiko::SkeletonPose m_externalPose;

    };

}

