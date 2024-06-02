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

        PboTextureComponent(string str, uint32_t width, uint32_t height, bool autoRender);
        PboTextureComponent();
        virtual ~PboTextureComponent() = default;

        virtual void init() override;
        virtual void update() override;
        virtual void render() override;

        void refreshPixels(bool = false);

        void updatePixel(uint16_t x, uint16_t y, Color c);
        void updatePixels(std::vector<Color> pixels);

        const texture::PboTexture& getPboTexture();

        bool auto_render = true;

    protected:

        texture::PboTexture m_texture;

    private:
        RenderSystem*       m_renderSystem;
        AikoPtr<Mesh>       m_mesh;
        AikoPtr<Shader>     m_shader;

        std::vector<Color> pixels;
        bool is_dirty = false;

    };

}