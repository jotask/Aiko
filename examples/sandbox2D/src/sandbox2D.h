#pragma once

#include "application/application.h"

namespace sandbox
{

    class Sandbox2D : public aiko::Application
    {
    protected:
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;
    private:
        aiko::GameObject* m_sprite;
    };

}

