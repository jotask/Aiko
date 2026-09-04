 #pragma once

#include <aiko_types.h>

#include "assets/types/texture_asset.h"
#include "types/texture_types.h"
#include "interfaces/itexture_impl.h"

namespace aiko
{
    class FrameBuffer;
    namespace renderer
    {
        class RendererFactory;
        class IRenderDevice;
    }

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
