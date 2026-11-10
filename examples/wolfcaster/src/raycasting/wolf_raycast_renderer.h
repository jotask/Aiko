#pragma once

#include "assets/asset_id.h"
#include "layers/contexts/ui_context.h"
#include "math/math_vector.h"

#include <vector>
#include <array>
#include <cstddef>
#include <cstdint>

namespace wc
{

    class WolfMap;
    class WolfPlayer;

    class WolfRaycastRenderer
    {
    public:
        static void render(const WolfMap& map, const WolfPlayer& player, const std::vector<aiko::AssetId>& wallTextures, const std::vector<aiko::AssetId>& spriteTextures, aiko::UIContext& ui, const aiko::vec2& viewportSize);

    private:

        static constexpr int32_t RayCount = 320;

        struct ProjectedSprite
        {
            size_t objectIndex;
            float depth;
            float centerX;
            float size;
        };

        static void renderSprites(const WolfMap& map, const WolfPlayer& player, const std::vector<aiko::AssetId>& spriteTextures, aiko::UIContext& ui, const aiko::vec2& viewportSize, float projectionDistance, const std::array<float, RayCount>& wallDepths, const aiko::SamplerState& sampler);
    };
}
