#pragma once

#include "application/application.h"

namespace aiko::ca
{

    class CellularAutomaton : public Application
    {
    protected:
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;
    private:
        GameObject* m_sprite;
    };

}

