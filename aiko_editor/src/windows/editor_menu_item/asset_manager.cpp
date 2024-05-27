#include "asset_manager.h"

#include <aiko_includes.h>

#include <fstream>
#include <streambuf>
#include <string>
#include <filesystem>
#include <imgui.h>
#include <json/json.h>
#include <imgui_internal.h>
#include <magic_enum.hpp>

namespace aiko
{
    namespace editor
    {

        AssetManager::AssetManager(AikoEditor* editor)
            : MenuItem(editor, "AssetManager")
        {
            load_file("assets.json");
        }

        void AssetManager::render()
        {
            constexpr const const char* Left_Window = "Left Window";
            constexpr const const char* Right_Window = "Right Window";

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse;
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
                            // TODO Open dialog to create new asset
                            for (size_t i = 0; i < 10; i++)
                            {
                                const std::string text = aiko::utils::generateRandomString();
                                assets.push_back({ uuid::Uuid(), text , text , Asset::AssetType::Default });
                            }
                        }
                        if (ImGui::MenuItem("Save", nullptr))
                        {
                            this->save_file();
                        }
                        if (ImGui::MenuItem("Load", nullptr))
                        {
                            this->load_file("assets.json");
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
                static long  selected_index = -1;
                if (ImGui::Begin(Left_Window, nullptr, window_flags))
                {
                    for (long i = 0 ; i < assets.size() ; i++)
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

                static auto drawInputText = [&](const char* title, std::string& str, bool read_only = false)
                    {
                        if (read_only)
                        {
                            ImGui::InputText(title, str.data(), str.size(), ImGuiInputTextFlags_ReadOnly);
                        }
                        else
                        {
                            ImGui::InputText(title, str.data(), str.size());
                        }
                    };

                // Create right child window
                if (ImGui::Begin(Right_Window, nullptr, window_flags))
                {
                    ImGui::Text("Asset Settings");
                    if (selected_index != -1)
                    {
                        Asset& asset = assets[selected_index];
                        drawInputText("Uuid", &asset.uid, true);
                        drawInputText("Name", asset.name);
                        drawInputText("Path", asset.path);

                        if (ImGui::BeginCombo("##combo", magic_enum::enum_name(asset.type).data() )) // The second parameter is the label previewed before opening the combo.
                        {
                            for (int n = 0; n < magic_enum::enum_count<Asset::AssetType>(); n++)
                            {
                                Asset::AssetType it = magic_enum::enum_value<Asset::AssetType>(n);
                                bool is_selected = (asset.type == it ); // You can store your selection however you want, outside or inside your objects
                                if (ImGui::Selectable(magic_enum::enum_name(it).data(), is_selected))
                                {
                                    asset.type = it;
                                }
                                if (is_selected)
                                {
                                    ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
                                }
                            }
                            ImGui::EndCombo();
                        }

                    }
                }
                ImGui::End();

            }
            ImGui::End();
        }

        void AssetManager::load_file(const char* file_name)
        {

            this->file_path = global::getAssetPath(file_name);

            assets.clear();

            std::ifstream file(file_path);
            if (file.is_open() == false)
            {
                aiko::Log::error("Error opening file: ", file_name );
                return;
            }

            if (file.peek() == EOF)
            {
                aiko::Log::error("Error opening file: ", file_name);
                return;
            }

            Json::Value root;
            file >> root;

            for (const auto& item : root) {
                Asset s =
                {
                    item["uuid"].asString(),
                    item["name"].asString(),
                    item["path"].asString(),
                    magic_enum::enum_cast<Asset::AssetType>(item["type"].asString()).value(),
                };
                assets.push_back(s);
            }

        }

        void AssetManager::save_file()
        {
            Json::Value root;
            for (std::vector<Asset>::iterator it = assets.begin(); it != assets.end(); it++)
            {
                Json::Value jsonVect;
                jsonVect["uuid"] = it->uid.get();
                jsonVect["name"] = it->name;
                jsonVect["path"] = it->path;
                jsonVect["type"] = magic_enum::enum_name(it->type).data();
                root.append(jsonVect);
            }
            Json::StyledWriter writer;
            std::string output = writer.write(root);

            // Save to file
            std::ofstream outfile;
            outfile.open(this->file_path, std::ios_base::trunc);//std::ios_base::app
            outfile << output;
            outfile.close();
        }

    }
}
