#pragma once

#include <aiko_includes.h>
#include <application/application.h>

namespace regression
{

    class RenderRegression : public aiko::Layer
    {
    protected:
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;
    private:

    };

}

