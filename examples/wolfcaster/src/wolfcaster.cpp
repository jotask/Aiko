#include "wolfcaster.h"

#include "core/file.h"
#include "layers/contexts/asset_context.h"
#include "layers/contexts/input_context.h"
#include "layers/contexts/render_context.h"
#include "layers/contexts/scene_context.h"
#include "layers/contexts/ui_context.h"
#include "raycasting/wolf_raycaster.h"
#include "rendering/wolf_map_texture.h"

#include <components/camera_component.h>
#include <components/mesh_component.h>
#include <intrumentor/profiler.h>
#include <magic_enum/magic_enum.hpp>
#include <types/draw_types.h>

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
        std::optional<WolfMap> wolfmap = m_mapFileReader.load(mapHeadPath, gameMapsPath);
        AIKO_ASSERT(wolfmap != std::nullopt, "Failed to load file map data");

        m_wolfMap = std::move(wolfmap);

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

        constexpr float MapScaler = 256.0f;

        // Geometry Plane Map
        if (m_mapTextureId != aiko::InvalidAssetId)
        {
            aiko::SamplerState state = {
                .minFilter = aiko::TextureFilter::Nearest,
                .magFilter = aiko::TextureFilter::Nearest,
                .mipFilter = aiko::TextureMipFilter::None,
                .wrapU = aiko::TextureWrapMode::Clamp,
                .wrapV = aiko::TextureWrapMode::Clamp,
            };
            const aiko::vec2 position = {0.0f, 0.0f};
            const aiko::vec2 size = { MapScaler };
            const aiko::Color tint = aiko::WHITE;
            ui().image(m_mapTextureId, position, size, tint, state);
        }

        // Player
        {
            const aiko::vec2 tileSize = { MapScaler / m_wolfMap->getWidth(), MapScaler / m_wolfMap->getHeight() };
            const aiko::vec2 playerScreenPosition = m_player.position() * tileSize;
            const aiko::vec2 playerSize = tileSize / 2.0f;
            const aiko::vec2 playerPositionOffset = playerScreenPosition - playerSize * 0.5f;
            ui().rect(playerPositionOffset, playerSize, aiko::RED);

            const aiko::vec2 forwardLength = tileSize * 3.0f;
            ui().line(playerScreenPosition, playerScreenPosition + ( m_player.forward() * forwardLength), 1.0f, aiko::MAGENTA);

            const WolfRayHit hit = WolfRaycaster::castRay(m_wolfMap.value(), m_player.position(), m_player.forward());
            if (hit.hit)
            {
                ui().line(playerScreenPosition, hit.position * tileSize, 1.0f, aiko::CYAN);
            }

            // Camera
            {

                constexpr int32_t RayCount = 128;

                constexpr aiko::vec2 viewportSize = {512, 384};
                constexpr aiko::vec2 viewportPosition = {0, 272};
                const float columnWidth = viewportSize.x / RayCount;

                constexpr float FOV = 66; // degrees
                const float planeLength = std::tan(aiko::math::radians(FOV) * 0.5f);
                const aiko::vec2 cameraPlane = m_player.right() * planeLength;

                for (int32_t rayIndex = 0 ; rayIndex < RayCount; ++rayIndex)
                {
                    const float cameraX = 2 * ( (rayIndex + 0.5f) / RayCount ) - 1.0f;
                    const aiko::vec2 rayDirection = m_player.forward() + cameraPlane * cameraX;
                    const WolfRayHit hit = WolfRaycaster::castRay(m_wolfMap.value(), m_player.position(), rayDirection);
                    if (hit.hit)
                    {
                        ui().line(playerScreenPosition, hit.position * tileSize, 1.0f, aiko::CYAN);

                        const auto normalizedRayDirection = aiko::math::normalize(rayDirection);
                        const float perpendicularDistance = hit.distance * aiko::math::dot(normalizedRayDirection, m_player.forward());

                        const float wallHeight = viewportSize.x / ( 2.0f * planeLength * perpendicularDistance );

                        const float columnX = viewportPosition.x + rayIndex * columnWidth;
                        const float columnY = viewportPosition.y + (viewportSize.y - wallHeight) / 2.0f;

                        const aiko::vec2 columnPosition = {columnX, columnY};
                        const aiko::vec2 columnSize = {columnWidth, wallHeight};
                        ui().rect(columnPosition, columnSize, aiko::LIGHTGRAY);

                    }
                }

            }
        }


    }

}

