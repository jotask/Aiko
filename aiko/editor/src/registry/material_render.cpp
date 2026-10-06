#include "material_render.h"

#include <models/material.h>

#include <imgui.h>
#include <magic_enum/magic_enum.hpp>

namespace aiko::editor::component
{
    namespace
    {
        template<typename T>
        bool drawEnum(const char* label, T& value)
        {
            bool changed = false;

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
                        changed = true;
                    }

                    if (selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }

                ImGui::EndCombo();
            }
            return changed;
        }
    }

    bool drawMaterial(Material& material)
    {
        bool changed = false;

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
            changed = true;
        }

        changed |= ImGui::Checkbox("Lit", &material.m_lit);

        changed |= ImGui::Checkbox("Use Vertex Color", &material.m_useVertexColor);

        ImGui::Spacing();

        if (ImGui::TreeNode("Render State"))
        {
            RenderState& state = material.m_renderState;
            changed |= drawEnum( "Cull Mode", state.cullMode);
            changed |= drawEnum("Fill Mode", state.fillMode);
            changed |= ImGui::Checkbox("Depth Test", &state.depthTest);
            changed |= ImGui::Checkbox("Depth Write", &state.depthWrite);
            if (state.depthTest)
            {
                changed |= drawEnum("Depth Compare", state.depthCompare);
            }
            changed |= ImGui::Checkbox("Blend", &state.blend);
            ImGui::TreePop();
        }

        ImGui::PopID();
        return changed;
    }
}
