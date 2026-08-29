#include "bgfx_imgui_aiko.h"

#include <bgfx/bgfx.h>
#include "platform/bgfx/imgui/bgfx_imgui_aiko.h"
#include "platform/bgfx/bgfx_types.h"

#include <imgui.h>

namespace aiko::renderer::bgfx
{

    void BgfxImguiImpl::init(const ViewId viewId, GLFWwindow* window)
    {
        ImGui_ImplGlfw_InitForOther(window, true);
        ImGui_Implbgfx_Init(viewId);
    }

    void BgfxImguiImpl::beginFrame(const ViewId viewId, int width, int height)
    {
        ImGui_Implbgfx_NewFrame();
    }

    void BgfxImguiImpl::endFrame(const ViewId viewId, int width, int height)
    {
        ::bgfx::setViewFrameBuffer(viewId, AIKO_INVALID_HANDLE);
        ::bgfx::setViewRect(viewId, 0, 0, static_cast<uint16_t>(width), static_cast<uint16_t>(height));
        ::bgfx::touch(viewId);

        ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());
    }

    void BgfxImguiImpl::dispose()
    {
        ImGui_Implbgfx_Shutdown();
        ImGui_ImplGlfw_Shutdown();
    }

}
