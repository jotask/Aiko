#pragma once

#include "application/application.h"

namespace sandbox
{

    class Sandbox : public aiko::Application
    {
    protected:
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;
    private:
        aiko::GameObject* m_go1;
        aiko::GameObject* m_go2;
    };

}

