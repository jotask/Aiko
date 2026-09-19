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

    struct UITheme
    {
        UIImageStyle image;
        UISelectableStyle selectable;
    };

}
