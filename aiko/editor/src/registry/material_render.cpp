#include "material_render.h"

#include <models/material.h>

#include <imgui.h>
#include <magic_enum/magic_enum.hpp>

namespace aiko::editor::component
{
    namespace
    {
        template<typename T>
        void drawEnum(const char* label, T& value)
        {
            const auto preview = magic_enum::enum_name(value);

            if (ImGui::BeginCombo(label, preview.data()))
            {
                for (const T current : magic_enum::enum_values<T>())
                {
                    const bool selected = current == value;
                    const auto name = magic_enum::enum_name(current);

                    if (ImGui::Selectable(name.data(), selected))
                    {
                        value = current;
                    }

                    if (selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }

                ImGui::EndCombo();
            }
        }
    }

    void drawMaterial(Material& material)
    {
        ImGui::PushID(&material);

        float baseColor[4] =
        {
            material.m_baseColor.r,
            material.m_baseColor.g,
            material.m_baseColor.b,
            material.m_baseColor.a
        };

        if (ImGui::ColorEdit4("Base Color", baseColor))
        {
            material.m_baseColor =
            {
                baseColor[0],
                baseColor[1],
                baseColor[2],
                baseColor[3]
            };
        }

        ImGui::Checkbox("Lit", &material.m_lit);

        ImGui::Checkbox("Use Vertex Color", &material.m_useVertexColor);

        ImGui::Spacing();

        if (ImGui::TreeNode("Render State"))
        {
            RenderState& state = material.m_renderState;
            drawEnum( "Cull Mode", state.cullMode);
            drawEnum("Fill Mode", state.fillMode);
            ImGui::Checkbox("Depth Test", &state.depthTest);
            ImGui::Checkbox("Depth Write", &state.depthWrite);
            if (state.depthTest)
            {
                drawEnum("Depth Compare", state.depthCompare);
            }
            ImGui::Checkbox("Blend", &state.blend);
            ImGui::TreePop();
        }

        ImGui::PopID();
    }
}
