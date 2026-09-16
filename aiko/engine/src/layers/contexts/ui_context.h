#pragma once

#include <math/math.h>
#include <types/color.h>

namespace aiko
{

    class SystemConnector;
    class RenderSystem;

    class UIContext
    {
    public:

        void rect(const vec2& position, const vec2& size, Color color);

    private:

        friend class LayerContext;

        explicit UIContext(SystemConnector& connector);

        RenderSystem* m_renderSystem = nullptr;
    };

}