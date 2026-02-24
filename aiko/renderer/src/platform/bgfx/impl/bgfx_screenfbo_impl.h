#pragma once

#include <aiko_types.h>
#include <math/math.h>

#include "interfaces/iscreenfbo_impl.h"
#include "models/frame_buffer.h"

#include <bgfx/bgfx.h>

namespace aiko::renderer::bgfx
{

    class BgfxScreenFboImpl : public interfaces::IScreenFboImpl
    {
    public:

        BgfxScreenFboImpl();
        virtual ~BgfxScreenFboImpl() override = default;

        virtual uint id() const override;
        virtual void use() override;
        virtual void unuse() override;
        virtual bool isValid() const override;

        // load
        virtual void create(int width, int height) override;
        virtual void unload() override;

        // getter
        virtual ivec2 getViewportSize() const override;

        FrameBuffer getFrameBuffer() const { return m_frameBuffer; }

    private:

        uint vao;
        uint vbo;
        FrameBuffer m_frameBuffer;

    };
}
