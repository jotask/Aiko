#include "editor_style.h"

#include <imgui.h>

namespace aiko::editor
{
    void applyEditorStyle()
    {
         ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        // ---------- Layout ----------
        style.WindowPadding     = ImVec2(10.0f, 10.0f);
        style.FramePadding      = ImVec2(8.0f, 6.0f);
        style.CellPadding       = ImVec2(8.0f, 6.0f);
        style.ItemSpacing       = ImVec2(8.0f, 6.0f);
        style.ItemInnerSpacing  = ImVec2(6.0f, 6.0f);
        style.TouchExtraPadding = ImVec2(0.0f, 0.0f);
        style.IndentSpacing     = 20.0f;
        style.ScrollbarSize     = 14.0f;
        style.GrabMinSize       = 10.0f;

        // ---------- Borders ----------
        style.WindowBorderSize = 1.0f;
        style.ChildBorderSize  = 1.0f;
        style.PopupBorderSize  = 1.0f;
        style.FrameBorderSize  = 0.0f;
        style.TabBorderSize    = 0.0f;

        // ---------- Rounding ----------
        style.WindowRounding    = 6.0f;
        style.ChildRounding     = 6.0f;
        style.FrameRounding     = 5.0f;
        style.PopupRounding     = 6.0f;
        style.ScrollbarRounding = 8.0f;
        style.GrabRounding      = 4.0f;
        style.TabRounding       = 5.0f;

        // ---------- Alignment ----------
        style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
        style.ButtonTextAlign  = ImVec2(0.5f, 0.5f);
        style.SelectableTextAlign = ImVec2(0.0f, 0.5f);

        // ---------- Dark editor palette ----------
        colors[ImGuiCol_Text]                 = ImVec4(0.86f, 0.86f, 0.88f, 1.00f);
        colors[ImGuiCol_TextDisabled]         = ImVec4(0.45f, 0.47f, 0.50f, 1.00f);

        colors[ImGuiCol_WindowBg]             = ImVec4(0.10f, 0.105f, 0.11f, 1.00f);
        colors[ImGuiCol_ChildBg]              = ImVec4(0.12f, 0.125f, 0.13f, 1.00f);
        colors[ImGuiCol_PopupBg]              = ImVec4(0.12f, 0.125f, 0.13f, 1.00f);

        colors[ImGuiCol_Border]               = ImVec4(0.22f, 0.23f, 0.25f, 1.00f);
        colors[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

        colors[ImGuiCol_FrameBg]              = ImVec4(0.16f, 0.165f, 0.18f, 1.00f);
        colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.22f, 0.23f, 0.25f, 1.00f);
        colors[ImGuiCol_FrameBgActive]        = ImVec4(0.27f, 0.29f, 0.32f, 1.00f);

        colors[ImGuiCol_TitleBg]              = ImVec4(0.09f, 0.095f, 0.10f, 1.00f);
        colors[ImGuiCol_TitleBgActive]        = ImVec4(0.12f, 0.125f, 0.13f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.09f, 0.095f, 0.10f, 1.00f);

        colors[ImGuiCol_MenuBarBg]            = ImVec4(0.14f, 0.145f, 0.15f, 1.00f);

        colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.10f, 0.105f, 0.11f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.28f, 0.30f, 0.34f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.38f, 0.42f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.40f, 0.44f, 0.50f, 1.00f);

        colors[ImGuiCol_CheckMark]            = ImVec4(0.40f, 0.70f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrab]           = ImVec4(0.40f, 0.70f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.28f, 0.60f, 0.95f, 1.00f);

        colors[ImGuiCol_Button]               = ImVec4(0.20f, 0.205f, 0.22f, 1.00f);
        colors[ImGuiCol_ButtonHovered]        = ImVec4(0.28f, 0.30f, 0.34f, 1.00f);
        colors[ImGuiCol_ButtonActive]         = ImVec4(0.22f, 0.50f, 0.85f, 1.00f);

        colors[ImGuiCol_Header]               = ImVec4(0.20f, 0.205f, 0.22f, 1.00f);
        colors[ImGuiCol_HeaderHovered]        = ImVec4(0.25f, 0.27f, 0.30f, 1.00f);
        colors[ImGuiCol_HeaderActive]         = ImVec4(0.22f, 0.50f, 0.85f, 1.00f);

        colors[ImGuiCol_Separator]            = ImVec4(0.22f, 0.23f, 0.25f, 1.00f);
        colors[ImGuiCol_SeparatorHovered]     = ImVec4(0.35f, 0.38f, 0.42f, 1.00f);
        colors[ImGuiCol_SeparatorActive]      = ImVec4(0.22f, 0.50f, 0.85f, 1.00f);

        colors[ImGuiCol_ResizeGrip]           = ImVec4(0.22f, 0.50f, 0.85f, 0.20f);
        colors[ImGuiCol_ResizeGripHovered]    = ImVec4(0.22f, 0.50f, 0.85f, 0.67f);
        colors[ImGuiCol_ResizeGripActive]     = ImVec4(0.22f, 0.50f, 0.85f, 0.95f);

        colors[ImGuiCol_Tab]                  = ImVec4(0.14f, 0.145f, 0.15f, 1.00f);
        colors[ImGuiCol_TabHovered]           = ImVec4(0.22f, 0.50f, 0.85f, 0.80f);
        colors[ImGuiCol_TabSelected]          = ImVec4(0.18f, 0.19f, 0.21f, 1.00f);
        colors[ImGuiCol_TabDimmed]            = ImVec4(0.11f, 0.115f, 0.12f, 1.00f);
        colors[ImGuiCol_TabDimmedSelected]    = ImVec4(0.15f, 0.16f, 0.17f, 1.00f);

        colors[ImGuiCol_DockingPreview]       = ImVec4(0.22f, 0.50f, 0.85f, 0.70f);
        colors[ImGuiCol_DockingEmptyBg]       = ImVec4(0.10f, 0.105f, 0.11f, 1.00f);
    }
}