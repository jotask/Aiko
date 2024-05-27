#pragma once

#include "aiko_types.h"
#include "models/component.h"
#include "models/mesh.h"
#include "models/shader.h"

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

        const texture::Texture& getTexture() { return m_texture; };

    private:
        RenderSystem*       m_renderSystem;
        AikoPtr<Mesh>       m_mesh;
        AikoPtr<Shader>     m_shader;
        texture::Texture    m_texture;

    };

}