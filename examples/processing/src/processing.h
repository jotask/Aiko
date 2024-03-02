#pragma once

#include "application/application.h"

namespace processing
{

    class Processing : public aiko::Application
    {
    protected:
        virtual void init() override;

    };

}

