#pragma once

#include <aiko_includes.h>
#include <imgui.h>

namespace aiko::editor
{
    namespace imgui
    {

        static bool InputText(const char* label, aiko::string* str, ImGuiInputTextFlags flags = 0)
        {
            struct InputTextCallbackData
            {
                static int Callback(ImGuiInputTextCallbackData* data)
                {
                    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
                    {
                        // Resize string and update data pointers
                        aiko::string* str = (aiko::string*)data->UserData;
                        IM_ASSERT(data->Buf == str->c_str());
                        str->resize(data->BufTextLen);
                        data->Buf = (char*)str->c_str();
                    }
                    return 0;
                }
            };

            return ::ImGui::InputText(label, (char*)str->c_str(), str->capacity() + 1, flags | ImGuiInputTextFlags_CallbackResize, InputTextCallbackData::Callback, (void*)str);
        }

        static void Image(texture::Texture& texture)
        {
            ImVec2 textureSize = ImVec2(texture.width, texture.height);
            ImVec2 availableRegionSize = ImGui::GetContentRegionAvail();
            float posX = (availableRegionSize.x - textureSize.x) * 0.5f;
            float posY = (availableRegionSize.y - textureSize.y) * 0.5f;
            ImGui::BeginChild("Texture", ImVec2(textureSize.x, textureSize.y), ImGuiChildFlags_None);
            ImGui::SetCursorPosX((ImGui::GetWindowSize().x - textureSize.x) * 0.5f);
            ImGui::Image((ImTextureID)texture.id, textureSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::EndChild();
        }

    }
}
