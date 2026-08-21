#pragma once

#include <aiko_types.h>
#include <bgfx/bgfx.h>
#include <math/math.h>

#include <types/texture_types.h>

#include "interfaces/iframebuffer_impl.h"
#include "models/texture.h"

namespace aiko::renderer::bgfx
{

    class BgfxFrameBufferImpl : public interfaces::IFrameBufferImpl
    {
    public:
        BgfxFrameBufferImpl();

        virtual ~BgfxFrameBufferImpl() override = default;

        virtual RenderResourceId id() const override;

        virtual void use() override;

        virtual void unuse() override;

        virtual bool isValid() const override;

        // load
        virtual void create(Texture& color, Texture& depth) override;

        virtual void unload() override;

        ::bgfx::FrameBufferHandle getFrameBufferHandler() const { return m_frameBuffer; }

    private:

        ::bgfx::FrameBufferHandle m_frameBuffer;

    };
}
