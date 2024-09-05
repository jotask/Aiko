#pragma once

#include "aiko_types.h"
#include "models/component.h"
#include "models/mesh.h"
#include "models/shader.h"
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
        RenderSystem*       m_renderSystem;
        AikoPtr<Mesh>       m_mesh;
        AikoPtr<Shader>     m_shader;

        WorldCellularAutomaton m_world;

    };

}