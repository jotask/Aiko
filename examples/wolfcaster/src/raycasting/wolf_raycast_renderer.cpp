#include "wolf_raycast_renderer.h"

#include "core/converters.h"
#include "map/wolf_map.h"
#include "map/wolf_map_types.h"
#include "objects/wolf_door.h"
#include "objects/wolf_player.h"
#include "wolf_raycaster.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <initializer_list>

namespace wc
{

    void WolfRaycastRenderer::render(const WolfMap& map, const WolfPlayer& player, const std::vector<aiko::AssetId>& wallTextures, const std::vector<aiko::AssetId>& spriteTextures, const std::vector<aiko::AssetId>& hudTextures, aiko::UIContext& ui, const aiko::vec2& viewportSize)
    {

        constexpr aiko::vec2 viewportPosition = {0, 0};

        const float hudScale = viewportSize.x / float(OriginalScreenSize.x);
        const float hudHeight = OriginalHudSize.y * hudScale;
        const aiko::vec2 gameViewportSize = {viewportSize.x, viewportSize.y - hudHeight};

        // Background
        {
            constexpr aiko::Color ceilingColor = aiko::Color(89 / 255.0f, 107 / 255.0f, 121 / 255.0f);
            constexpr aiko::Color floorColor = aiko::Color(80 / 255.0f, 74 / 255.0f, 69 / 255.0f);

            ui.pushClipRect(viewportPosition, gameViewportSize);
            ui.rect({0}, { gameViewportSize.x, gameViewportSize.y * 0.5f }, ceilingColor);
            ui.rect({0, gameViewportSize.y * 0.5f}, gameViewportSize, floorColor);
            ui.popClipRect();
        }

        // World
        {

            constexpr float ReferenceHorizontalFOV = 66.0f; // degrees
            constexpr float ReferenceAspectRatio = 4.0f / 3.0f;

            const float columnWidth = gameViewportSize.x / RayCount;
            const float aspectRatio = gameViewportSize.x / gameViewportSize.y;
            const float referencePlaneLength = std::tan(aiko::math::radians(ReferenceHorizontalFOV) * 0.5f);
            const float planeLength = referencePlaneLength * (aspectRatio / ReferenceAspectRatio);
            const float projectionDistance = gameViewportSize.x / (2.0f * planeLength);
            const aiko::vec2 cameraPlane = player.right() * planeLength;

            constexpr aiko::SamplerState sampler = {
                .minFilter = aiko::TextureFilter::Nearest,
                .magFilter = aiko::TextureFilter::Nearest,
                .mipFilter = aiko::TextureMipFilter::None,
                .wrapU = aiko::TextureWrapMode::Clamp,
                .wrapV = aiko::TextureWrapMode::Clamp,
            };

            ui.pushClipRect(viewportPosition, gameViewportSize);

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
                    const float columnY = viewportPosition.y + (gameViewportSize.y - wallHeight) / 2.0f;

                    const aiko::vec2 columnPosition = {columnX, columnY};
                    const aiko::vec2 columnSize = {columnWidth, wallHeight};

                    if (hit.tileX < 0 || hit.tileX >= map.getWidth() || hit.tileY < 0 || hit.tileY >= map.getHeight())
                    {
                        // nothing?
                        continue;
                    }

                    const WolfDoor* pushWall = hit.kind == WolfRayHitKind::PushWall ? map.getActivePushWall() : nullptr;

                    AIKO_ASSERT(hit.kind != WolfRayHitKind::PushWall || pushWall != nullptr, "Missing active push-wall");
                    const TileId tileId = pushWall ? pushWall->wallTileId : map.getGeometryTile(hit.tileX, hit.tileY);

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

                    float wallU = 0.0f;
                    if (pushWall)
                    {
                        const aiko::vec2 wallMin = {
                            float(pushWall->tileCoord.x) + pushWall->pushDirection.x * pushWall->openProgress,
                            float(pushWall->tileCoord.y) + pushWall->pushDirection.y * pushWall->openProgress
                        };

                        wallU = hit.side == WolfRaySide::Vertical ? hit.position.y - wallMin.y : hit.position.x - wallMin.x;
                    }
                    else
                    {
                        switch (hit.side)
                        {
                            case WolfRaySide::Horizontal: wallU = hit.position.x; break;
                            case WolfRaySide::Vertical:   wallU = hit.position.y; break;
                            default:
                                AIKO_ASSERT(false, "Unknown wall side");
                        }
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

            const std::vector<ProjectedSprite> visiblesSprites = calculateVisibleSprites(map, player, spriteTextures, gameViewportSize, projectionDistance);
            renderSprites(visiblesSprites, gameViewportSize, ui, wallDepths, spriteTextures, sampler);

            ui.popClipRect();
        }

        renderWeapon(player, spriteTextures, ui, gameViewportSize);

        renderHUD(map, player, hudTextures, ui, viewportSize);

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

    void WolfRaycastRenderer::renderWeapon(const WolfPlayer& player, const std::vector<aiko::AssetId>& spriteTextures, aiko::UIContext& ui, const aiko::vec2& viewportSize)
    {

        size_t weaponBaseIndex = 0;

        switch (player.getWeapon())
        {
            case WolfWeapon::Knife: weaponBaseIndex = 416; break;
            case WolfWeapon::Pistol: weaponBaseIndex = 421; break;
            case WolfWeapon::MachineGun: weaponBaseIndex = 426; break;
            case WolfWeapon::GatlingGun: weaponBaseIndex = 431; break;
            default:
                AIKO_ASSERT(false, "Weapon not handled");
        }

        const size_t spriteIndex = weaponBaseIndex + player.weaponAnimationFrame();
        AIKO_ASSERT(spriteIndex < spriteTextures.size(), "Invalid weapon sprite index");

        const float weaponSize = viewportSize.y * 1.0f;

        const aiko::vec2 weaponPosition = {
            (viewportSize.x - weaponSize) * 0.5f,
            viewportSize.y - weaponSize
        };

        const aiko::vec2 weaponDimensions = { weaponSize, weaponSize};

        constexpr aiko::SamplerState sampler = {
            .minFilter = aiko::TextureFilter::Nearest,
            .magFilter = aiko::TextureFilter::Nearest,
            .mipFilter = aiko::TextureMipFilter::None,
            .wrapU = aiko::TextureWrapMode::Clamp,
            .wrapV = aiko::TextureWrapMode::Clamp,
        };

        ui.image(spriteTextures[spriteIndex], weaponPosition, weaponDimensions, aiko::WHITE, sampler);
    }

    void WolfRaycastRenderer::renderHUD(const WolfMap& map, const WolfPlayer& player, const std::vector<aiko::AssetId>& hudTextures, aiko::UIContext& ui, const aiko::vec2& viewportSize)
    {

        const float hudScale = viewportSize.x / float(OriginalScreenSize.x);
        const float hudHeight = OriginalHudSize.y * hudScale;

        const aiko::vec2 position = aiko::vec2	{0.0f, viewportSize.y - hudHeight};
        const aiko::vec2 size = aiko::vec2	{viewportSize.x, hudHeight};

        constexpr aiko::SamplerState sampler = {
            .minFilter = aiko::TextureFilter::Nearest,
            .magFilter = aiko::TextureFilter::Nearest,
            .mipFilter = aiko::TextureMipFilter::None,
            .wrapU = aiko::TextureWrapMode::Clamp,
            .wrapV = aiko::TextureWrapMode::Clamp,
        };

        // Player Status bar
        {
            constexpr size_t StatusBarPic = 95;
            AIKO_ASSERT(StatusBarPic < hudTextures.size(), "Missing status bar texture");
            AIKO_ASSERT(hudTextures[StatusBarPic] != aiko::InvalidAssetId, "Invalid status bar texture");
            const aiko::AssetId textureId = hudTextures[StatusBarPic];
            ui.image(textureId, position, size, aiko::WHITE, sampler);
        }

        constexpr size_t BlankIndex = 107;
        const auto getDigitIndex = [](uint8_t digit) -> size_t
        {
            AIKO_ASSERT(digit <= 9, "Digit must be between 0 and 9");
            return 108 + digit;
        };

        const auto renderHudNumber = [&](uint32_t value, uint8_t digitCount, const aiko::vec2& originalPosition)
        {
            AIKO_ASSERT(digitCount > 0 && digitCount <= 10, "Invalid HUD digit count");

            constexpr float DigitWidth = 8.0f;
            constexpr float DigitHeight = 16.0f;

            uint32_t divisor = 1;
            for (uint8_t i = 1; i < digitCount; ++i)
            {
                divisor *= 10;
            }

            for (uint8_t i = 0; i < digitCount; ++i)
            {
                const uint8_t digit = static_cast<uint8_t>((value / divisor) % 10);

                const bool leadingBlank = divisor > 1 && value < divisor;
                const size_t textureIndex = leadingBlank ? BlankIndex : getDigitIndex(digit);

                AIKO_ASSERT(textureIndex < hudTextures.size(), "Invalid HUD digit texture index");

                const aiko::AssetId textureId = hudTextures[textureIndex];
                AIKO_ASSERT(textureId != aiko::InvalidAssetId, "Invalid HUD digit texture");

                const aiko::vec2 digitPosition = {
                    position.x + (originalPosition.x + i * DigitWidth) * hudScale,
                    position.y + originalPosition.y * hudScale
                };

                const aiko::vec2 digitSize = { DigitWidth * hudScale, DigitHeight * hudScale};
                ui.image(textureId, digitPosition, digitSize, aiko::WHITE, sampler);
                divisor /= 10;
            }
        };

        const auto renderHudPicture = [&](size_t textureIndex, const aiko::vec2& originalPosition, const aiko::vec2& originalSize)
        {
            const aiko::vec2 picturePosition = { position.x + originalPosition.x * hudScale, position.y + originalPosition.y * hudScale };
            const aiko::vec2 pictureSize = {originalSize.x * hudScale, originalSize.y * hudScale};
            ui.image(hudTextures[textureIndex], picturePosition, pictureSize, aiko::WHITE, sampler);
        };

        // Player current level
        {
            const uint32_t levelNumber = map.getLevelIndex() + 1;
            renderHudNumber(levelNumber, 2, {16.0f, 16.0f});
        }

        // Player score
        {
            const uint32_t scoreNumber = player.score();
            renderHudNumber(scoreNumber, 6, {48.0f, 16.0f});
        }

        // Player lives
        {
            const uint32_t livesNumber = 8;
            renderHudNumber(livesNumber, 1, {112.0f, 16.0f});
        }

        // Player health
        {
            const uint32_t healthNumber = std::floor((player.health() / static_cast<float>(WolfPlayer::MaxHealth)) * 100.0f);
            renderHudNumber(healthNumber, 3, {168.0f, 16.0f});
        }

        // Player ammo
        {
            const uint32_t ammoNumber = player.ammo();
            renderHudNumber(ammoNumber, 2, {216.0f, 16.0f});
        }

        // Player Health Icon
        {
            const uint32_t healthNumber = std::floor((player.health() / static_cast<float>(WolfPlayer::MaxHealth)) * 100.0f);
            size_t faceIndex = 118;
            if (healthNumber == 0) { faceIndex = 139; }
            else if (healthNumber <= 3) { faceIndex = 136; }
            else if (healthNumber <= 19) { faceIndex = 133; }
            else if (healthNumber <= 35) { faceIndex = 130; }
            else if (healthNumber <= 51) { faceIndex = 127; }
            else if (healthNumber <= 67) { faceIndex = 124; }
            else if (healthNumber <= 83) { faceIndex = 121; }
            renderHudPicture(faceIndex, {136, 4}, {24, 32});
        }

        // Player keys
        {
            constexpr size_t NoKeyIndex = 104;
            constexpr size_t GoldKeyIndex = 105;
            constexpr size_t SilverKeyIndex = 106;

            renderHudPicture(NoKeyIndex, {240, 4}, {8, 16});
            renderHudPicture(NoKeyIndex, {240, 20}, {8, 16});
        }

        // Player weapon
        {
            size_t weaponIndex = 0;
            switch (player.getWeapon())
            {
                case WolfWeapon::Knife: weaponIndex = 100; break;
                case WolfWeapon::Pistol: weaponIndex = 101; break;
                case WolfWeapon::MachineGun: weaponIndex = 102; break;
                case WolfWeapon::GatlingGun: weaponIndex = 103; break;
            }
            renderHudPicture(weaponIndex, {256, 8}, {48, 24});
        }

    }

    size_t WolfRaycastRenderer::resolveEnemySpriteIndex(const WolfEnemy& enemy, const aiko::vec2& playerPosition)
    {

        const auto calculateSprite = [&](const size_t standing, const size_t walking, const size_t attacking, std::initializer_list<size_t> deathSprites) -> size_t
        {
            if (enemy.animation() == WolfEnemyAnimation::Death)
            {
                AIKO_ASSERT(enemy.animationFrame() < deathSprites.size(), "Invalid death animation frame");
                return deathSprites.begin()[enemy.animationFrame()];
            }

            if (enemy.animation() == WolfEnemyAnimation::Attack)
            {
                return attacking + enemy.animationFrame();
            }

            const uint8_t angleOffset = calculateEnemyRotation(enemy, playerPosition);

            if (enemy.animation() == WolfEnemyAnimation::Idle)
            {
                return standing + angleOffset;
            }

            return walking + enemy.animationFrame() * 8 + angleOffset;
        };

        switch (enemy.type())
        {
            case WolfEnemyType::Guard:              return calculateSprite(50, 58, 96, {91, 92, 93, 95});
            case WolfEnemyType::Dog:                return calculateSprite(99, 99, 135, {131, 132, 133, 134});
            case WolfEnemyType::SS:                 return calculateSprite(138, 146, 184, {179, 180, 181, 183});
            case WolfEnemyType::Mutant:             return calculateSprite(187, 195, 234, {228, 229, 230, 232, 233});
            case WolfEnemyType::Officer:            return calculateSprite(238, 246, 285, {279, 280, 281, 283, 284});
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

    uint8_t WolfRaycastRenderer::calculateEnemyRotation(const WolfEnemy& enemy, const aiko::vec2& playerPosition)
    {
        const aiko::vec2 direction = playerPosition - enemy.position();
        const float angleTowardsPlayer = std::atan2(-direction.y, direction.x);
        const float enemyFacing = convertDirectionToAngle(enemy.direction());
        float relative = std::fmod(angleTowardsPlayer - enemyFacing, 2 * aiko::PI);
        if (relative < 0)
        {
            relative += 2 * aiko::PI;
        }
        const int sectorIndex = ( ( relative + ( aiko::PI / 8 )  ) / (aiko::PI / 4) );
        return uint8_t(sectorIndex % 8);

    }

}
