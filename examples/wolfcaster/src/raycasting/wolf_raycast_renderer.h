#pragma once

#include "assets/asset_id.h"
#include "layers/contexts/ui_context.h"
#include "math/math_vector.h"

#include <vector>

namespace wc
{

    class WolfMap;
    class WolfPlayer;

    class WolfRaycastRenderer
    {
    public:
        static void render(const WolfMap& map, const WolfPlayer& player, const std::vector<aiko::AssetId>& wallTextures, const std::vector<aiko::AssetId>& spriteTextures, aiko::UIContext& ui, const aiko::vec2& viewportSize);
    };
}
