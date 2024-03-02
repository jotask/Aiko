#pragma once

#include "application/application.h"

namespace raylib
{

    class Raylib : public aiko::Application
    {
    protected:
        virtual void init() override;

    };

}

