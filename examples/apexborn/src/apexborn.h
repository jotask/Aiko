#pragma once

#include <aiko_includes.h>
#include <layers/layer.h>

#include "world/track.h"

namespace ab
{

    class ApexBorn : public aiko::Layer
    {
    public:
        ApexBorn();
        ~ApexBorn();

    protected:

        virtual void init() override;
        virtual void update() override;
        virtual void render() override;
    private:

        void renderImgui();
        void renderMenuBar();

        Track m_track;

    };

}

