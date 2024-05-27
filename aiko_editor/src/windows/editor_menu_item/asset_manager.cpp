#include "asset_manager.h"

#include <aiko_includes.h>

#include <imgui.h>
#include <imgui_internal.h>

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
            constexpr const const char* Left_Window = "Left Window";
            constexpr const const char* Right_Window = "Right Window";

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
            if (ImGui::Begin("AssetManager", &is_open, window_flags))
            {
                // Create a docking space inside the main window
                ImGuiID dockspace_id = ImGui::GetID("AssetManagerDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoTabBar);

                ImVec2 dockspace_size = ImGui::GetWindowSize();

                if (ImGui::BeginMenuBar())
                {
                    if (ImGui::BeginMenu("File"))
                    {
                        if (ImGui::MenuItem("New", nullptr))
                        {
                            // TODO Create new asset?
                            for (size_t i = 0 ; i < 10; i++)
                            {
                                const std::string text = aiko::utils::generateRandomString();
                                assets.push_back({ uuid::Uuid(), text , text , text});
                            }
                        }
                        if (ImGui::MenuItem("Close", nullptr))
                        {
                            is_open = false;
                        }
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenuBar();
                }

                // DockBuilder code to automatically dock the child windows
                static bool dock_initialized = false;
                if (dock_initialized == false)
                {
                    dock_initialized = true;

                    ImGui::DockBuilderRemoveNode(dockspace_id); // Clear any previous layout
                    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
                    ImGui::DockBuilderSetNodeSize(dockspace_id, dockspace_size);

                    ImGuiID dock_main_id = dockspace_id;
                    ImGuiID left_id, right_id;
                    ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.2f, &left_id, &right_id);

                    ImGui::DockBuilderDockWindow(Left_Window, left_id);
                    ImGui::DockBuilderDockWindow(Right_Window, right_id);

                    ImGui::DockBuilderFinish(dockspace_id);
                }

                constexpr const ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav;

                // Create left child window
                static size_t  selected_index = -1;
                if (ImGui::Begin(Left_Window, nullptr, window_flags))
                {
                    for (size_t i = 0 ; i < assets.size() ; i++)
                    {
                        if (ImGui::Selectable(assets[i].name.c_str(), selected_index == i))
                        {
                            if (selected_index == i)
                            {
                                selected_index = -1;
                            }
                            else
                            {
                                selected_index = i;
                            }
                        }
                    }
                }
                ImGui::End();

                // Create right child window
                if (ImGui::Begin(Right_Window, nullptr, window_flags))
                {
                    ImGui::Text("Asset Settings");
                    if (selected_index != -1)
                    {
                        auto asset = assets[selected_index];
                        ImGui::Text("Name: %s ", asset.name.c_str());
                        ImGui::Text("Path: %s ", asset.path.c_str());
                        ImGui::Text("Type: %s ", asset.type.c_str());
                    }
                }
                ImGui::End();

            }
            ImGui::End();
        }

    }
}
