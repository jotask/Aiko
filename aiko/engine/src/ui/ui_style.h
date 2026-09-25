#pragma once

#include <optional>

#include "assets/asset_id.h"
#include "models/texture_region.h"
#include <types/color.h>

namespace aiko
{

    struct UIImageAppearance
    {
        Color color = WHITE;
        std::optional<AssetId> texture;
        std::optional<TextureRegion> textureRegion;
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
