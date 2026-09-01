#include "cellular_automaton_component.h"

namespace aiko::ca
{

    CellularAutomatonComponent::CellularAutomatonComponent()
        : Component("CellularAutomaton")
    {

    }

    void CellularAutomatonComponent::init()
    {
        m_world.init();
    }

    WorldCellularAutomaton& CellularAutomatonComponent::getWorld()
    {
        return m_world;
    }

}
