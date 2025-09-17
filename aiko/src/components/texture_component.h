#pragma once

#include "aiko_types.h"
#include "models/component.h"
#include "models/mesh.h"
#include "models/shader.h"
#include "models/texture.h"

namespace aiko
{

    class RenderSystem;

    class TextureComponent : public Component, public IUpdate, public IRender3D
    {
    public:

        friend class RenderSystem;

        TextureComponent();
        virtual ~TextureComponent() = default;

        virtual void init() override;
        virtual void update() override;
        virtual void render() override;

        const Texture* getTexture() { return &m_texture; };

    private:
        RenderSystem* m_renderSystem;
        Mesh       m_mesh;
        Shader     m_shader;
        Texture    m_texture;

    };

}