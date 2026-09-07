 #pragma once

#include <metadata/texture_meta.h>
#include <types/color.h>
#include <types/render_types.h>
#include <types/texture_types.h>

namespace aiko::interfaces
{
    class ITextureImpl;
}

namespace aiko
{
    class FrameBuffer;
    namespace renderer
    {
        class RendererFactory;
        class IRenderDevice;
    }

    struct TextureAsset;

    class Texture
    {
        friend class FrameBuffer;
        friend class renderer::IRenderDevice;
        friend class renderer::RendererFactory;
    public:

        // Copy
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        // Move
        Texture(Texture&&) noexcept = delete;
        Texture& operator=(Texture&&) noexcept = delete;

        Texture();
        virtual ~Texture();

        bool isValid() const;
        RenderResourceId id() const;
        TextureInfo getInfo() const;

        // load
        void create();
        void create(int width, int height);
        void create(const TextureDesc& desc);
        void upload(const TextureAsset& asset);
        void update(const TextureAsset& asset);
        void unload();

        // Modify
        virtual void setPixels(const vector<Color>&);

    private:

        AikoPtr<interfaces::ITextureImpl> backend;

    };

}
