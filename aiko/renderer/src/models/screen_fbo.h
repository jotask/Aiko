 #pragma once

#include <aiko_types.h>
#include <math/math.h>

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
        ~ScreenFbo() = default;

        void use();
        void unuse();
        bool isValid();
        uint id() const;
        void destroy();

        // load
        void create(int width, int height);
        void unload();

        // getters
        ivec2 getViewportSize() const;

        void* getImpl() const { return backend.get(); }

    private:

        AikoPtr<interfaces::IScreenFboImpl> backend;

    };

}
