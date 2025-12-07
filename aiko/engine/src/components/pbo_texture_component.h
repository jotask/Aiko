#pragma once

#include <aiko_types.h>
#include <models/mesh.h>
#include <models/shader.h>
#include <models/texture.h>

#include "models/component.h"

namespace aiko
{

    class PboTextureComponent : public Component, public IUpdate, public IRender3D
    {
    public:

        friend class RenderSystem;

        PboTextureComponent(string str, uint32_t width, uint32_t height, bool autoRender);
        PboTextureComponent();
        virtual ~PboTextureComponent() = default;

        virtual void init() override;
        virtual void update() override;
        virtual void render() override;

        void refreshPixels(bool = false);

        void updatePixel(uint16_t x, uint16_t y, Color c);
        void updatePixels(std::vector<Color> pixels);

        const Texture getTexture();

        bool auto_render = true;

    protected:

        Texture m_texture;

    private:

        Mesh       m_mesh;
        Shader     m_shader;

        std::vector<Color> pixels;
        bool is_dirty = false;

    };

}