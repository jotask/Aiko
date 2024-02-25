#pragma once

#include "application/application.h"

namespace sandbox
{

    class Sandbox : public aiko::Application
    {
    protected:
        virtual void init() override;

    };

}

