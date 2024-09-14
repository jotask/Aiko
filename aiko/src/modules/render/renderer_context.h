#pragma once

#include "shared/math.h"
#include "types/textures.h"

namespace aiko
{
    class RenderModule;
    class RenderContext
    {
    public:
        RenderContext(RenderModule* renderer);
        virtual ~RenderContext() = default;

        virtual void init() = 0;
        virtual void beginFrame() = 0;
        virtual void endFrame() = 0;
        virtual void dispose() = 0;

    protected:
        RenderModule* getRenderModule();

    private:
        RenderModule* m_renderModule;


    };
}
