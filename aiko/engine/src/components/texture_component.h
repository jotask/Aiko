#pragma once

#include <aiko_types.h>
#include <models/mesh.h>
#include <models/shader.h>
#include <models/texture.h>

#include "models/component.h"

namespace aiko
{

    class RenderSystem;

    class TextureComponent : public Component, public IUpdate, public IRender3D
    {
    public:

        enum class TextureMode { FILE, PBO };

        friend class RenderSystem;

        TextureComponent();
        TextureComponent(string file);
        TextureComponent(TextureMode);
        virtual ~TextureComponent() = default;

        virtual void init() override;
        virtual void update() override;
        virtual void render() override;

        void load(string);

        const Texture getTexture() { return m_texture; };

        // TODO TMP for now
        void setPixel(uint16_t x, uint16_t y, Color c);
        void setPixels(std::vector<Color> pixels);
        void refresh();

    private:
        RenderSystem* m_renderSystem;
        Mesh       m_mesh;
        Shader     m_shader;
        Texture    m_texture;

        TextureMode m_textureMode;
        string     m_filePath;

        std::vector<Color> pixels;
        bool is_dirty = false;

    };

}