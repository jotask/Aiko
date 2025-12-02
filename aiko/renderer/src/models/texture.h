 #pragma once

#include <aiko_types.h>

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

        void use();
        void unuse();
        bool isValid();
        uint id() const;

        // load
        void create();
        void create(int width, int height);
        void load(const char*);
        void unload();

    private:

        AikoPtr<interfaces::ITextureImpl> backend;

    };

}
