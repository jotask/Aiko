#pragma once

#include "assets/asset_id.h"
#include "layers/contexts/ui_context.h"
#include "math/math_vector.h"
#include "objects/wolf_enemy.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

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
            size_t spriteIndex;
            float depth;
            float centerX;
            float size;
        };

        static std::vector<ProjectedSprite> calculateVisibleSprites(const WolfMap& map, const WolfPlayer& player, const std::vector<aiko::AssetId>& spriteTextures, const aiko::vec2& viewportSize, float projectionDistance);
        static void renderSprites(const std::vector<ProjectedSprite>& visibleSprites, const aiko::vec2& viewportSize, aiko::UIContext& ui, const std::array<float, RayCount>& wallDepths, const std::vector<aiko::AssetId>& spriteTextures, const aiko::SamplerState& sampler);

        static void renderWeapon(const WolfPlayer& player, const std::vector<aiko::AssetId>& spriteTextures, aiko::UIContext& ui, const aiko::vec2& viewportSize);

        static size_t resolveEnemySpriteIndex(const WolfEnemy& enemy, const aiko::vec2& playerPosition);
        static uint8_t calculateEnemyRotation(const WolfEnemy& enemy, const aiko::vec2& playerPosition);

    };
}
