#pragma once

#include "imgui/aiko_imgui.h"

namespace aiko::renderer::bgfx
{

    class BgfxImguiImpl : public AikoImguiImpl
    {
    public:
        virtual void init(const ViewId id, GLFWwindow*) override;
        virtual void beginFrame(int width, int height) override;
        virtual void endFrame(int width, int height) override;
    };

}

