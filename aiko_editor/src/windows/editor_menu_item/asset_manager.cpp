#include "asset_manager.h"

#include <imgui.h>

namespace aiko
{
    namespace editor
    {

        AssetManager::AssetManager(AikoEditor* editor)
            : MenuItem(editor, "AssetManager")
        {

        }

        void AssetManager::render()
        {
            if (ImGui::Begin("AssetManager", &is_open))
            {
                ImGui::Text("Yop");
            }
            ImGui::End();
        }

    }
}
