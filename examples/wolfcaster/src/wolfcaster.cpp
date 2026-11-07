#include "wolfcaster.h"

#include "assets/wolf_map_texture.h"
#include "assets/wolf_vswap_reader.h"
#include "assets/wolf_wall_texture_decoder.h"
#include "core/file.h"
#include "layers/contexts/asset_context.h"
#include "layers/contexts/input_context.h"
#include "layers/contexts/render_context.h"
#include "layers/contexts/scene_context.h"
#include "layers/contexts/ui_context.h"
#include "map/wolf_map_reader.h"
#include "raycasting/wolf_raycaster.h"

#include <components/camera_component.h>
#include <components/mesh_component.h>
#include <intrumentor/profiler.h>
#include <magic_enum/magic_enum.hpp>
#include <types/draw_types.h>

#include <algorithm>
#include <imgui.h>
#include <random>
#include <vector>

namespace wc
{

    WolfCaster::WolfCaster() = default;

    WolfCaster::~WolfCaster() = default;

    void WolfCaster::init()
    {

        scene().clearColor() = {0.10f, 0.16f, 0.13f};

        aiko::CameraComponent* camera = scene().createCamera(aiko::camera::CameraController::Fly, aiko::Camera::CameraType::Perspective);
        camera->getCamera().position = { 0.0f, 50.0f, 10.0f };
        camera->getCamera().target = {0.0f, 0.0f, 0.0f};
        camera->speed() *= 4.0f;

        scene().ambientLight().color = aiko::WHITE;
        scene().ambientLight().intensity = 0.3f;

        const std::filesystem::path dataDir = WOLF3D_DATA_DIR;

        const auto mapHeadPath = dataDir / "MAPHEAD.WL1";
        const auto gameMapsPath = dataDir / "GAMEMAPS.WL1";

        std::optional<WolfMap> wolfMap = WolfMapReader::load(mapHeadPath, gameMapsPath);
        AIKO_ASSERT(wolfMap != std::nullopt, "Failed to load file map data");

        m_wolfMap = std::move(wolfMap);

        m_doorSystem.initialize(m_wolfMap.value());

        const WolfVSwapData assetData = WolfVSwapReader::load(dataDir / "VSWAP.WL1");
        for (const WolfWallTexture& wallTexture : assetData.walls)
        {
            const aiko::TextureAsset asset = WolfWallTextureDecoder::decode(wallTexture);
            const aiko::AssetId assetId = assets().createTexture(asset);
            m_wallsTexture.push_back(assetId);
        }

        const aiko::TextureAsset asset = WolfMapTexture::build(m_wolfMap.value());
        m_mapTextureId = assets().createTexture(asset);

        const WolfPlayerSpawn playerSpawn = m_wolfMap->findPlayerSpawn();
        aiko::logger::Log::infoFmt("WolfCaster Player Spawn");
        aiko::logger::Log::infoFmt("     Tile: ({} , {})", playerSpawn.tileX, playerSpawn.tileY);
        aiko::logger::Log::infoFmt("     Direction: {}", magic_enum::enum_name(playerSpawn.direction));

        m_player = WolfPlayer::fromSpawn(playerSpawn);

    }

    void WolfCaster::update()
    {
        AIKO_FUNCTION_PROFILE

        const float deltaTime = getDeltaTime();

        constexpr float rotationSpeed = 2.5f;
        constexpr float moveSpeed = 3.0f;

        if (input().isKeyPressed(aiko::Key::KEY_A) == true)
        {
            m_player.rotate(-rotationSpeed * deltaTime);
        }
        if (input().isKeyPressed(aiko::Key::KEY_D) == true)
        {
            m_player.rotate(rotationSpeed * deltaTime);
        }

        aiko::vec2 movementDirection = {0.0f};

        if (input().isKeyPressed(aiko::Key::KEY_W) == true)
        {
            movementDirection += m_player.forward();
        }
        if (input().isKeyPressed(aiko::Key::KEY_S) == true)
        {
            movementDirection -= m_player.forward();
        }

        if (input().isKeyPressed(aiko::Key::KEY_Q) == true)
        {
            movementDirection -= m_player.right();
        }
        if (input().isKeyPressed(aiko::Key::KEY_E) == true)
        {
            movementDirection += m_player.right();
        }

        if (movementDirection != 0)
        {
            const aiko::vec2 normalizedMove = aiko::math::normalize(movementDirection);
            const aiko::vec2 displacement = normalizedMove * moveSpeed * deltaTime;
            m_player.move(displacement, m_wolfMap.value());
        }

    }

    void WolfCaster::render()
    {
        AIKO_FUNCTION_PROFILE
        renderBackground();
        renderRaycaster();

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Tools"))
            {
                ImGui::MenuItem("Render MiniMap", nullptr, &m_debugImgui.renderMiniMap);
                ImGui::MenuItem("Render Wall Textures", nullptr, &m_debugImgui.renderWallTextures);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        renderMiniMap();
        renderAllWallTexture();

    }

    void WolfCaster::renderBackground()
    {
        constexpr aiko::Color ceilingColor = aiko::Color(89 / 255.0f, 107 / 255.0f, 121 / 255.0f);
        constexpr aiko::Color floorColor = aiko::Color(80 / 255.0f, 74 / 255.0f, 69 / 255.0f);

        const auto renderSize = renderer().getRenderSize();

        constexpr aiko::vec2 viewportPosition = {0, 0};
        const aiko::vec2 viewportSize = {renderSize.x * 1.0f, renderSize.y * 1.0f};

        ui().pushClipRect(viewportPosition, viewportSize);
        ui().rect({0}, { viewportSize.x, viewportSize.y * 0.5f }, ceilingColor);
        ui().rect({0, viewportSize.y * 0.5f}, viewportSize, floorColor);
        ui().popClipRect();

    }

    void WolfCaster::renderMiniMap()
    {
        if (m_debugImgui.renderMiniMap == false)
        {
            return;
        }

        if (ImGui::Begin("Mini Map", &m_debugImgui.renderMiniMap))
        {
            const aiko::SamplerState sampler = {
                .minFilter = aiko::TextureFilter::Nearest,
                .magFilter = aiko::TextureFilter::Nearest,
                .mipFilter = aiko::TextureMipFilter::None,
                .wrapU = aiko::TextureWrapMode::Clamp,
                .wrapV = aiko::TextureWrapMode::Clamp,
            };

            const ImVec2 available = ImGui::GetContentRegionAvail();

            if (available.x > 0.0f && available.y > 0.0f)
            {
                const float mapWidth = static_cast<float>(m_wolfMap->getWidth());
                const float mapHeight = static_cast<float>(m_wolfMap->getHeight());

                const float scale = std::min( available.x / mapWidth, available.y / mapHeight);
                const ImVec2 imageSize = { mapWidth * scale, mapHeight * scale };

                const ImVec2 cursor = ImGui::GetCursorPos();
                ImGui::SetCursorPos({ cursor.x + (available.x - imageSize.x) * 0.5f, cursor.y + (available.y - imageSize.y) * 0.5f});
                const ImTextureID textureId = static_cast<ImTextureID>(renderer().getTextureId(m_mapTextureId, sampler));
                ImGui::Image(textureId, imageSize);

                // Get the actual position of the minimap image.
                const ImVec2 imagePosition = ImGui::GetItemRectMin();

                const float scaleX = imageSize.x / static_cast<float>(m_wolfMap->getWidth());
                const float scaleY = imageSize.y / static_cast<float>(m_wolfMap->getHeight());

                // Player position in minimap coordinates.
                const aiko::vec2 playerPosition = m_player.position();

                const ImVec2 playerScreenPosition = { imagePosition.x + playerPosition.x * scaleX, imagePosition.y + playerPosition.y * scaleY };

                // Forward direction.
                const aiko::vec2 forward = m_player.forward();

                constexpr float directionLength = 3.0f; // In map tiles

                const ImVec2 forwardEnd = {
                    playerScreenPosition.x + forward.x * directionLength * scaleX,
                    playerScreenPosition.y + forward.y * directionLength * scaleY
                };

                // Draw overlays on top of the minimap.
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                drawList->PushClipRect(imagePosition, ImVec2(imagePosition.x + imageSize.x, imagePosition.y + imageSize.y), true);

                // Forward direction.
                drawList->AddLine(playerScreenPosition, forwardEnd, IM_COL32(255, 0, 255, 255), 2.0f);

                // Player marker.
                drawList->AddCircleFilled(playerScreenPosition, 4.0f, IM_COL32(255, 0, 0, 255));

                drawList->PopClipRect();
            }

        }

        ImGui::End();
    }

    void WolfCaster::renderRaycaster()
    {
        constexpr int32_t RayCount = 320;
        constexpr float FOV = 66; // degrees

        const aiko::ivec2 renderSize = renderer().getRenderSize();

        const aiko::vec2 viewportSize = {renderSize.x * 1.0f, renderSize.y * 1.0f};
        const aiko::vec2 viewportPosition = {0, 0};

        const float columnWidth = viewportSize.x / RayCount;

        const float planeLength = std::tan(aiko::math::radians(FOV) * 0.5f);
        const aiko::vec2 cameraPlane = m_player.right() * planeLength;

        constexpr aiko::SamplerState sampler = {
            .minFilter = aiko::TextureFilter::Nearest,
            .magFilter = aiko::TextureFilter::Nearest,
            .mipFilter = aiko::TextureMipFilter::None,
            .wrapU = aiko::TextureWrapMode::Clamp,
            .wrapV = aiko::TextureWrapMode::Clamp,
        };

        ui().pushClipRect(viewportPosition, viewportSize);

        for (int32_t rayIndex = 0 ; rayIndex < RayCount; ++rayIndex)
        {
            const float cameraX = 2 * ( (rayIndex + 0.5f) / RayCount ) - 1.0f;
            const aiko::vec2 rayDirection = m_player.forward() + cameraPlane * cameraX;
            const WolfRayHit hit = WolfRaycaster::castRay(m_wolfMap.value(), m_player.position(), rayDirection);
            if (hit.hit)
            {
                const auto normalizedRayDirection = aiko::math::normalize(rayDirection);
                const float perpendicularDistance = hit.distance * aiko::math::dot(normalizedRayDirection, m_player.forward());

                const float wallHeight = viewportSize.x / ( 2.0f * planeLength * perpendicularDistance );

                const float columnX = viewportPosition.x + rayIndex * columnWidth;
                const float columnY = viewportPosition.y + (viewportSize.y - wallHeight) / 2.0f;

                const aiko::vec2 columnPosition = {columnX, columnY};
                const aiko::vec2 columnSize = {columnWidth, wallHeight};

                if (hit.tileX < 0 || hit.tileX >= m_wolfMap->getWidth() || hit.tileY < 0 || hit.tileY >= m_wolfMap->getHeight())
                {
                    // nothing?
                    continue;
                }

                const auto tileId = m_wolfMap->getTile(WolfMapPlane::Geometry, hit.tileX, hit.tileY);

                uint16_t wallId = 0;

                switch (hit.side)
                {
                    case WolfRaySide::Horizontal: wallId = ( tileId - 1 ) * 2;      break;
                    case WolfRaySide::Vertical: wallId = ( tileId - 1 ) * 2 + 1;    break;
                    default: AIKO_ASSERT(false, "Unknow Hit Site");
                }

                AIKO_ASSERT(wallId < m_wallsTexture.size(), "OOB for wall texture");
                aiko::AssetId textureAssetId = m_wallsTexture[wallId];
                AIKO_ASSERT(textureAssetId != aiko::InvalidAssetId, "Invalid Asset Id");

                float wallU = 0;
                switch (hit.side)
                {
                    case WolfRaySide::Horizontal: wallU = hit.position.x; break;
                    case WolfRaySide::Vertical:   wallU = hit.position.y; break;
                    default: AIKO_ASSERT(false, "Unknow Hit Site");
                }

                const float fractionalU = wallU - std::floor(wallU);
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

                // const aiko::Color wallColor = hit.side == WolfRaySide::Vertical ? aiko::LIGHTGRAY : aiko::GRAY;
                // ui().rect(columnPosition, columnSize, wallColor);

                aiko::Color tint = aiko::WHITE;
                switch (hit.side)
                {
                    case WolfRaySide::Horizontal: tint = aiko::Color(0.75f, 0.75f, 0.75f, 1.0f); break;
                    case WolfRaySide::Vertical:   tint = aiko::WHITE; break;
                    default: AIKO_ASSERT(false, "Unknow Hit Site");
                }

                ui().image(textureAssetId, region, columnPosition, columnSize, tint, sampler);

            }
        }

        ui().popClipRect();

    }

    void WolfCaster::renderAllWallTexture()
    {
        if (m_debugImgui.renderWallTextures == false)
        {
            return;
        }

        if (ImGui::Begin("Wall Textures", &m_debugImgui.renderWallTextures))
        {
            const aiko::SamplerState sampler = {
                .minFilter = aiko::TextureFilter::Nearest,
                .magFilter = aiko::TextureFilter::Nearest,
                .mipFilter = aiko::TextureMipFilter::None,
                .wrapU = aiko::TextureWrapMode::Clamp,
                .wrapV = aiko::TextureWrapMode::Clamp,
            };

            constexpr float textureSize = 64.0f;

            const float availableWidth = ImGui::GetContentRegionAvail().x;
            const float spacing = ImGui::GetStyle().ItemSpacing.x;

            const int columns = std::max(1, static_cast<int>((availableWidth + spacing) / (textureSize + spacing)));

            for (size_t i = 0; i < m_wallsTexture.size(); ++i)
            {
                const aiko::AssetId assetId = m_wallsTexture[i];

                if (assetId == aiko::InvalidAssetId)
                {
                    continue;
                }

                const ImTextureID textureId = static_cast<ImTextureID>(renderer().getTextureId(assetId, sampler));

                ImGui::BeginGroup();

                ImGui::Text("Wall %zu", i);
                ImGui::Image(textureId, ImVec2(textureSize, textureSize));

                ImGui::EndGroup();

                if ((i + 1) % columns != 0 && i + 1 < m_wallsTexture.size())
                {
                    ImGui::SameLine();
                }
            }
        }

        ImGui::End();
    }

}

