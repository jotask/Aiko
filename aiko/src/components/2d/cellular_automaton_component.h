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
        texture::PboTexture m_texture;

        std::vector<Color> pixels;
        bool is_dirty = false;

        void refreshPixels(bool = false);
        void updatePixel(uint16_t x, uint16_t y, Color c);
        void updatePixels(std::vector<Color> pixels);

        WorldCellularAutomaton m_world;

    };

}