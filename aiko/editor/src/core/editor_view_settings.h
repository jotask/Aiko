#pragma once

#include <types/color.h>

namespace aiko::editor
{
    struct EditorViewSettings
    {
        bool showLightGizmos = true;
        bool showGrid = true;

        bool overrideClearColor = false;
        Color clearColor = Color(0.15f, 0.15f, 0.15f, 1.0f);
    };
}