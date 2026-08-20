#pragma once

#include "imgui/aiko_imgui.h"

namespace aiko::renderer::vulkan
{

    class VulkanImguiImpl : public AikoImguiImpl
    {
    public:
        virtual void init(const ViewId id, GLFWwindow*) override;
        virtual void beginFrame(const ViewId id, int width, int height) override;
        virtual void endFrame(const ViewId id, int width, int height) override;
        virtual void dispose() override;
    };

}

