#pragma once

#include "aiko_types.h"
#include "models/component.h"
#include "cellular_automaton/world_cellular_automaton.h"

namespace aiko
{
    namespace ca
    {

        class CellularAutomatonComponent : public aiko::Component
        {
        public:

            CellularAutomatonComponent();
            virtual ~CellularAutomatonComponent() = default;

            virtual void init() override;

            WorldCellularAutomaton& getWorld();

        private:

            WorldCellularAutomaton m_world;

        };

    }
}