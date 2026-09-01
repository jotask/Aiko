#pragma once

#include "application/application.h"
#include "cell_automaton_component/cellular_automaton/automaton_renderer.h"

namespace aiko::ca
{

    class CellularAutomatonComponent;

    class CellularAutomaton : public Layer
    {
    protected:
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;
    private:
        CellularAutomatonComponent* m_automaton = nullptr;
        cellautomaton::AutomatonRender m_renderer;
    };

}

