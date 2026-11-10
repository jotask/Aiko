#include "wolf_raycast_renderer.h"

#include "objects/wolf_player.h"
#include "map/wolf_map_types.h"
#include "map/wolf_map.h"
#include "objects/wolf_door.h"
#include "wolf_raycaster.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace wc
{

    void WolfRaycastRenderer::render(const WolfMap& map, const WolfPlayer& player, const std::vector<aiko::AssetId>& wallTextures, const std::vector<aiko::AssetId>& spriteTextures, aiko::UIContext& ui, const aiko::vec2& viewportSize)
    {

        constexpr aiko::vec2 viewportPosition = {0, 0};

        // Background
        {
            constexpr aiko::Color ceilingColor = aiko::Color(89 / 255.0f, 107 / 255.0f, 121 / 255.0f);
            constexpr aiko::Color floorColor = aiko::Color(80 / 255.0f, 74 / 255.0f, 69 / 255.0f);


            ui.pushClipRect(viewportPosition, viewportSize);
            ui.rect({0}, { viewportSize.x, viewportSize.y * 0.5f }, ceilingColor);
            ui.rect({0, viewportSize.y * 0.5f}, viewportSize, floorColor);
            ui.popClipRect();
        }

        // World
        {

            constexpr float ReferenceHorizontalFOV = 66.0f; // degrees
            constexpr float ReferenceAspectRatio = 4.0f / 3.0f;

            const aiko::vec2 viewportPosition = {0, 0};

            const float columnWidth = viewportSize.x / RayCount;
            const float aspectRatio = viewportSize.x / viewportSize.y;
            const float referencePlaneLength = std::tan(aiko::math::radians(ReferenceHorizontalFOV) * 0.5f);
            const float planeLength = referencePlaneLength * (aspectRatio / ReferenceAspectRatio);
            const float projectionDistance = viewportSize.x / (2.0f * planeLength);
            const aiko::vec2 cameraPlane = player.right() * planeLength;

            constexpr aiko::SamplerState sampler = {
                .minFilter = aiko::TextureFilter::Nearest,
                .magFilter = aiko::TextureFilter::Nearest,
                .mipFilter = aiko::TextureMipFilter::None,
                .wrapU = aiko::TextureWrapMode::Clamp,
                .wrapV = aiko::TextureWrapMode::Clamp,
            };

            ui.pushClipRect(viewportPosition, viewportSize);

            std::array<float, RayCount> wallDepths;
            wallDepths.fill(std::numeric_limits<float>::infinity());

            for (int32_t rayIndex = 0 ; rayIndex < RayCount; ++rayIndex)
            {
                const float cameraX = 2 * ( (rayIndex + 0.5f) / RayCount ) - 1.0f;
                const aiko::vec2 rayDirection = player.forward() + cameraPlane * cameraX;
                const WolfRayHit hit = WolfRaycaster::castRay(map, player.position(), rayDirection);
                if (hit.hit)
                {
                    const auto normalizedRayDirection = aiko::math::normalize(rayDirection);
                    const float perpendicularDistance = hit.distance * aiko::math::dot(normalizedRayDirection, player.forward());
                    if (std::isfinite(perpendicularDistance) == false || perpendicularDistance <= 0.0f)
                    {
                        continue;
                    }

                    const float wallHeight = projectionDistance / perpendicularDistance;

                    const float columnX = viewportPosition.x + rayIndex * columnWidth;
                    const float columnY = viewportPosition.y + (viewportSize.y - wallHeight) / 2.0f;

                    const aiko::vec2 columnPosition = {columnX, columnY};
                    const aiko::vec2 columnSize = {columnWidth, wallHeight};

                    if (hit.tileX < 0 || hit.tileX >= map.getWidth() || hit.tileY < 0 || hit.tileY >= map.getHeight())
                    {
                        // nothing?
                        continue;
                    }

                    const TileId tileId = map.getGeometryTile(hit.tileX, hit.tileY);

                    uint16_t wallId = 0;

                    const bool vertical = hit.side == WolfRaySide::Vertical;

                    if (hit.kind == WolfRayHitKind::Door)
                    {
                        const size_t doorBase = wallTextures.size() - 8;
                        size_t doorOffset = 0;
                        if (tileId >= 92 && tileId <= 99)
                        {
                            // Locked door
                            doorOffset = 6;
                        }
                        else if (tileId >= 100 && tileId <= 101)
                        {
                            // Elevator door
                            doorOffset = 4;
                        }
                        wallId = doorBase + doorOffset + ( vertical ? 1 : 0 );
                    }
                    else
                    {
                        if (tileId < 1 || tileId > 63)
                        {
                            continue;
                        }
                        wallId = (static_cast<size_t>(tileId) - 1) * 2 + (vertical ? 1 : 0);
                    }

                    AIKO_ASSERT(wallId < wallTextures.size(), "OOB for wall texture");
                    aiko::AssetId textureAssetId = wallTextures[wallId];
                    AIKO_ASSERT(textureAssetId != aiko::InvalidAssetId, "Invalid Asset Id");

                    float wallU = 0;
                    switch (hit.side)
                    {
                        case WolfRaySide::Horizontal: wallU = hit.position.x; break;
                        case WolfRaySide::Vertical:   wallU = hit.position.y; break;
                        default: AIKO_ASSERT(false, "Unknow Hit Site");
                    }

                    float fractionalU = wallU - std::floor(wallU);

                    if (hit.kind == WolfRayHitKind::Door)
                    {
                        const WolfDoor* door = map.getDoor(hit.tileX, hit.tileY);
                        fractionalU -= door->openProgress;
                    }

                    int textureX = aiko::math::clamp(static_cast<int>(fractionalU * 64.0f), 0, 63);

                    if ((hit.side == WolfRaySide::Vertical && rayDirection.x < 0) || ( hit.side == WolfRaySide::Horizontal && rayDirection.y > 0 ))
                    {
                        textureX = 63 - textureX;
                    }

                    const float u = (static_cast<float>(textureX) + 0.5f) / 64.0f;
                    const aiko::TextureRegion region{
                        .min = {u, 0.0f},
                        .max = {u, 1.0f}
                    };

                    aiko::Color tint = aiko::WHITE;
                    switch (hit.side)
                    {
                        case WolfRaySide::Horizontal: tint = aiko::Color(0.75f, 0.75f, 0.75f, 1.0f); break;
                        case WolfRaySide::Vertical:   tint = aiko::WHITE; break;
                        default: AIKO_ASSERT(false, "Unknow Hit Site");
                    }

                    wallDepths[rayIndex] = perpendicularDistance;

                    ui.image(textureAssetId, region, columnPosition, columnSize, tint, sampler);

                }
            }

            const std::vector<ProjectedSprite> visiblesSprites = calculateVisibleSprites(map, player, spriteTextures, viewportSize, projectionDistance);
            renderSprites(visiblesSprites, viewportSize, ui, wallDepths, spriteTextures, sampler);

            ui.popClipRect();
        }

    }

    std::vector<WolfRaycastRenderer::ProjectedSprite> WolfRaycastRenderer::calculateVisibleSprites(const WolfMap& map, const WolfPlayer& player, const std::vector<aiko::AssetId>& spriteTextures, const aiko::vec2& viewportSize, float projectionDistance)
    {

        const aiko::vec2 forward = player.forward();
        const aiko::vec2 right = player.right();

        const auto& objects = map.getObjects();
        const auto& enemies = map.getEnemies();

        std::vector<ProjectedSprite> visibleSprites;
        visibleSprites.reserve(objects.size() + enemies.size());

        const auto projectSprite = [&](const aiko::vec2& position, size_t spriteIndex )
        {
            if (spriteIndex >= spriteTextures.size())
            {
                return;
            }

            const aiko::vec2 relative = position - player.position();
            const float depth = aiko::math::dot(relative, forward);

            if (depth <= 0.05f)
            {
                return;
            }

            const float lateral = aiko::math::dot(relative, right);
            const float centerX = viewportSize.x * 0.5f + lateral * projectionDistance / depth;
            const float size = projectionDistance / depth;
            const float left = centerX - size * 0.5f;
            const float rightEdge = centerX + size * 0.5f;

            if (rightEdge <= 0.0f || left >= viewportSize.x)
            {
                return;
            }

            visibleSprites.push_back({
                .spriteIndex = spriteIndex,
                .depth = depth,
                .centerX = centerX,
                .size = size
            });
        };

        for (const WolfWorldObject& object : map.getObjects())
        {
            projectSprite(object.position, object.spriteIndex);
        }

        for (const WolfEnemy& enemy : map.getEnemies())
        {
            const size_t spriteIdx = resolveEnemySpriteIndex(enemy, player.position());
            projectSprite(enemy.position(), spriteIdx);
        }

        // Transparent sprites must be rendered farthest first.
        std::sort(
            visibleSprites.begin(),
            visibleSprites.end(),
            [](const ProjectedSprite& a, const ProjectedSprite& b)
            {
                return a.depth > b.depth;
            }
        );

        return visibleSprites;

    }

    void WolfRaycastRenderer::renderSprites(const std::vector<ProjectedSprite>& visibleSprites, const aiko::vec2& viewportSize, aiko::UIContext& ui, const std::array<float, RayCount>& wallDepths, const std::vector<aiko::AssetId>& spriteTextures, const aiko::SamplerState& sampler)
    {

        const float columnWidth = viewportSize.x / RayCount;

        for (const ProjectedSprite& sprite : visibleSprites)
        {
            const aiko::AssetId textureId = spriteTextures[sprite.spriteIndex];

            if (textureId == aiko::InvalidAssetId)
            {
                continue;
            }

            const float left = sprite.centerX - sprite.size * 0.5f;
            const float rightEdge = sprite.centerX + sprite.size * 0.5f;
            const float top = (viewportSize.y - sprite.size) * 0.5f;

            const float clippedLeft = std::max(0.0f, left);
            const float clippedRight = std::min(viewportSize.x, rightEdge);

            const int firstColumn = static_cast<int>(std::floor(clippedLeft / columnWidth));
            const int lastColumn = std::min(RayCount - 1, static_cast<int>(std::ceil(clippedRight / columnWidth)) - 1);

            for (int column = firstColumn; column <= lastColumn; ++column)
            {
                if (sprite.depth >= wallDepths[column])
                {
                    continue;
                }

                const float screenX = (column + 0.5f) * columnWidth;
                if (screenX < left || screenX >= rightEdge)
                {
                    continue;
                }

                const float normalizedU = (screenX - left) / sprite.size;
                const int textureX = std::clamp(static_cast<int>(normalizedU * 64.0f), 0, 63);
                const float u = (textureX + 0.5f) / 64.0f;

                const aiko::TextureRegion region{
                    .min = {u, 0.0f},
                    .max = {u, 1.0f}
                };

                ui.image(textureId, region, {column * columnWidth, top}, {columnWidth, sprite.size}, aiko::WHITE, sampler);
            }
        }
    }

    size_t WolfRaycastRenderer::resolveEnemySpriteIndex(const WolfEnemy& enemy, const aiko::vec2& playerPosition)
    {
        switch (enemy.type())
        {
            case WolfEnemyType::Guard:              return (enemy.spawnMode() == WolfEnemySpawnMode::Standing) ? 50 : 58;
            case WolfEnemyType::Dog:                return (enemy.spawnMode() == WolfEnemySpawnMode::Standing) ? 99 : 99;
            case WolfEnemyType::SS:                 return (enemy.spawnMode() == WolfEnemySpawnMode::Standing) ? 138 : 146;
            case WolfEnemyType::Mutant:             return (enemy.spawnMode() == WolfEnemySpawnMode::Standing) ? 187 : 195;
            case WolfEnemyType::Officer:            return (enemy.spawnMode() == WolfEnemySpawnMode::Standing) ? 238 : 246;
            case WolfEnemyType::FakeHitler:	        return 321;
            case WolfEnemyType::MechaHitler:	    return 334;
            case WolfEnemyType::GeneralFettgesicht:	return 396;
            case WolfEnemyType::DrSchabbs:	        return 307;
            case WolfEnemyType::GretelGrosse:	    return 385;
            case WolfEnemyType::HansGrosse:	        return 296;
            case WolfEnemyType::OttoGiftmacher:	    return 360;
            case WolfEnemyType::Blinky:	            return 288;
            case WolfEnemyType::Clyde:	            return 292;
            case WolfEnemyType::Pinky:	            return 290;
            case WolfEnemyType::Inky:	            return 294;
        }
        AIKO_ASSERT(false, "Unknow sprite for unknow enemy type");
        return {};
    }

}
