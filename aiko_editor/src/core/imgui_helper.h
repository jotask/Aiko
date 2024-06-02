#pragma once

#include <aiko_includes.h>
#include <imgui.h>
#include <vector>

namespace editor
{
    namespace ImGui
    {

        bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0)
        {
            struct InputTextCallbackData
            {
                static int Callback(ImGuiInputTextCallbackData* data)
                {
                    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
                    {
                        // Resize std::string and update data pointers
                        std::string* str = (std::string*)data->UserData;
                        IM_ASSERT(data->Buf == str->c_str());
                        str->resize(data->BufTextLen);
                        data->Buf = (char*)str->c_str();
                    }
                    return 0;
                }
            };

            return ::ImGui::InputText(label, (char*)str->c_str(), str->capacity() + 1, flags | ImGuiInputTextFlags_CallbackResize, InputTextCallbackData::Callback, (void*)str);
        }

    }
}
