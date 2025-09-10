#pragma once

#include "aiko_types.h"
#include "models/component.h"
#include "cellular_automaton/automaton_renderer.h"
#include "cellular_automaton/world_cellular_automaton.h"

namespace aiko
{
    namespace ca
    {

        class CellularAutomatonComponent : public aiko::Component, public aiko::IUpdate, public aiko::IRender2D
        {
        public:

            CellularAutomatonComponent();
            virtual ~CellularAutomatonComponent() = default;

            virtual void init() override;
            virtual void update() override;
            virtual void render() override;

            WorldCellularAutomaton& getWorld();

        private:

            WorldCellularAutomaton m_world;
            cellautomaton::AutomatonRender m_render;

        };

    }
}