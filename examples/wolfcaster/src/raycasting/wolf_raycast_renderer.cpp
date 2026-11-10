#include "wolf_raycast_renderer.h"

#include "objects/wolf_player.h"
#include "map/wolf_map_types.h"
#include "map/wolf_map.h"
#include "objects/wolf_door.h"
#include "wolf_raycaster.h"

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
            constexpr int32_t RayCount = 320;

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

            for (int32_t rayIndex = 0 ; rayIndex < RayCount; ++rayIndex)
            {
                const float cameraX = 2 * ( (rayIndex + 0.5f) / RayCount ) - 1.0f;
                const aiko::vec2 rayDirection = player.forward() + cameraPlane * cameraX;
                const WolfRayHit hit = WolfRaycaster::castRay(map, player.position(), rayDirection);
                if (hit.hit)
                {
                    const auto normalizedRayDirection = aiko::math::normalize(rayDirection);
                    const float perpendicularDistance = hit.distance * aiko::math::dot(normalizedRayDirection, player.forward());

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

                    ui.image(textureAssetId, region, columnPosition, columnSize, tint, sampler);

                }
            }

            ui.popClipRect();
        }

    }

}