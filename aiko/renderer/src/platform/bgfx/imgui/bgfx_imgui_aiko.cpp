#include "bgfx_imgui_aiko.h"

#include <bgfx/bgfx.h>
#include "platform/bgfx/imgui/bgfx_imgui_aiko.h"
#include "platform/bgfx/bgfx_types.h"

namespace aiko::renderer::bgfx
{

    void BgfxImguiImpl::init(const ViewId id, GLFWwindow* window)
    {
        ImGui_ImplGlfw_InitForOther(window, true);
        ImGui_Implbgfx_Init(id);
    }

    void BgfxImguiImpl::beginFrame(int width, int height)
    {
        ImGui_Implbgfx_NewFrame();
    }

    void BgfxImguiImpl::endFrame(int width, int height)
    {
        ::bgfx::setViewFrameBuffer(m_viewId, AIKO_INVALID_HANDLE);
        ::bgfx::setViewRect(m_viewId, 0, 0, static_cast<uint16_t>(width), static_cast<uint16_t>(height));
        ::bgfx::touch(m_viewId);

        ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());
    }
}
