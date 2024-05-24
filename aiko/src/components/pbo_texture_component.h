#pragma once

#include "aiko_types.h"
#include "models/component.h"
#include "models/mesh.h"
#include "models/shader.h"

namespace aiko
{

    class RenderSystem;

    class PboTextureComponent : public Component, public IUpdate, public IRender3D
    {
    public:

        friend class RenderSystem;

        PboTextureComponent();
        virtual ~PboTextureComponent() = default;

        virtual void init() override;
        virtual void update() override;
        virtual void render() override;

        void updatePixel(uint16_t x, uint16_t y, Color c);

    private:
        RenderSystem*       m_renderSystem;
        AikoPtr<Mesh>       m_mesh;
        AikoPtr<Shader>     m_shader;
        texture::PboTexture m_texture;

        std::vector<Color> pixels;
        bool is_dirty = false;

    };

}