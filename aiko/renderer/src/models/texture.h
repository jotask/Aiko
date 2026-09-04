 #pragma once

#include <aiko_types.h>

#include "assets/types/texture_asset.h"
#include "types/texture_types.h"
#include "interfaces/itexture_impl.h"

namespace aiko
{
    namespace renderer
    {
        class RendererFactory;
    }

    class Texture
    {
    public:

        friend class renderer::RendererFactory;

        // Copy
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        // Move
        Texture(Texture&&) noexcept = delete;
        Texture& operator=(Texture&&) noexcept = delete;

        Texture();
        virtual ~Texture();

        void* getImpl() const { return backend.get(); }

        void use();
        void unuse();
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
