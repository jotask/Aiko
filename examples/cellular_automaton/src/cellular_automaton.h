#pragma once

#include "application/application.h"
#include "cell_automaton_component/cellular_automaton/automaton_renderer.h"
#include "cell_automaton_component/cellular_automaton/world_cellular_automaton.h"
#include "layers/layer.h"

namespace aiko::ca
{

    class CellularAutomaton : public Layer
    {
    protected:
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;
    private:
        WorldCellularAutomaton m_world;
        AutomatonRender m_renderer;
    };

}

