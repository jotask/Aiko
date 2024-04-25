#include "game_window.h"

#include "aiko_editor.h"
#include "aiko.h"
#include "systems/render_system.h"

#include "window.h"

namespace aiko
{
    namespace editor
    {

        GameWindow::GameWindow(AikoEditor* editor)
            : Window(editor)
        {

        }

        void GameWindow::render()
        {

            static auto* renderSystem = m_editor->getAiko()->getSystem<aiko::RenderSystem>();

            /*
            if(ImGui::Begin("Game"))
            {
                auto* aikoTarget = renderSystem->getTargetTexture();  // Get the size of the child (i.e. the whole draw size of the windows).
                auto target = raylib::utils::toRaylibRenderTexture2D(*aikoTarget);
                ImVec2 lastSize = ImGui::GetWindowSize();
                rlImGuiImageRect(&target.texture, target.texture.width, target.texture.height, ::Rectangle{ 0,0, lastSize.x, -lastSize.y });
                ImGui::End();
            }
            */
        }

    }
}
