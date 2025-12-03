 #pragma once

#include <aiko_types.h>

#include "models/texture.h"
#include "interfaces/iscreenfbo_impl.h"

namespace aiko
{
    namespace renderer
    {
        class RendererFactory;
    }

    class ScreenFbo
    {
    public:

        friend class renderer::RendererFactory;

        // Copy
        ScreenFbo(const ScreenFbo&) = default;
        ScreenFbo& operator=(const ScreenFbo&) = default;

        // Move
        ScreenFbo(ScreenFbo&&) noexcept = default;
        ScreenFbo& operator=(ScreenFbo&&) noexcept = default;

        ScreenFbo();
        virtual ~ScreenFbo() = default;

        virtual void use() = 0;
        virtual void unuse() = 0;
        virtual bool isValid() = 0;
        virtual uint id() const = 0;

        // load
        virtual void create(int width, int height) = 0;
        virtual void unload() = 0;

    private:

        AikoPtr<interfaces::IScreenFboImpl> backend;

        uint framebuffer;
        Texture texture;
        Texture depth;

    };

}
