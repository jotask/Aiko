#pragma once

#include "aiko_types.h"
#include "models/component.h"
#include "components/2d/cellular_automaton/automaton_chunk_renderer.h"
#include "components/2d/cellular_automaton/world_cellular_automaton.h"

namespace aiko
{

    class RenderSystem;

    class CellularAutomatonComponent : public Component, public IUpdate, public IRender3D
    {
    public:

        friend class RenderSystem;

        CellularAutomatonComponent();
        virtual ~CellularAutomatonComponent() = default;

        virtual void init() override;
        virtual void update() override;
        virtual void render() override;

    private:

        const bool fps_timer_update = true;

        WorldCellularAutomaton m_world;
        cellautomaton::AutomatonRender m_render;

    };

}