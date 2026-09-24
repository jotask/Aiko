#pragma once

#include <types/color.h>

namespace aiko
{

    struct UIImageAppearance
    {
        Color color = WHITE;
    };

    struct UIImageStyle
    {
        UIImageAppearance appearance;
    };

    struct UISelectableStyle
    {
        UIImageAppearance normal;
        UIImageAppearance hovered;
        UIImageAppearance pressed;
        UIImageAppearance disabled;
    };

    struct UIButtonStyle
    {
        UISelectableStyle selectable;
    };

    struct UITheme
    {
        UIImageStyle image;

        UISelectableStyle selectable;

        UIButtonStyle button;
    };

}
