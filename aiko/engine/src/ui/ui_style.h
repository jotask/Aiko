#pragma once

#include <optional>

#include "assets/asset_id.h"
#include "models/texture_region.h"
#include <types/color.h>

namespace aiko
{

    struct UIBorder
    {
        float thickness = 0.0f;
        Color color = WHITE;
    };

    struct UIImageAppearance
    {
        Color color = WHITE;
        std::optional<AssetId> texture;
        std::optional<TextureRegion> textureRegion;
        UIBorder border;
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
