 #pragma once

#include <aiko_types.h>

#include "types/textures.h"
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
        Texture(const Texture&) = default;
        Texture& operator=(const Texture&) = default;

        // Move
        Texture(Texture&&) noexcept = default;
        Texture& operator=(Texture&&) noexcept = default;

        Texture();
        virtual ~Texture() = default;

        void* getImpl() const { return backend.get(); }

        void use();
        void unuse();
        bool isValid() const;
        uint id() const;
        texture::Texture getInfo() const;

        // load
        void create();
        void create(int width, int height);
        void load(string);
        void unload();

        // Modify
        virtual void setPixels(std::vector<Color>&);

    private:

        AikoPtr<interfaces::ITextureImpl> backend;

    };

}
