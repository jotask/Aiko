#pragma once

#include <layers/layer.h>

#include <aiko_includes.h>

namespace wc
{

    class WolfCaster : public aiko::Layer
    {
    public:
        WolfCaster();
        ~WolfCaster();

    protected:
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;
        
    private:

    };

}

