#include "wolfcaster.h"

#include "core/file.h"
#include "layers/contexts/asset_context.h"
#include "layers/contexts/render_context.h"
#include "layers/contexts/scene_context.h"
#include "layers/contexts/ui_context.h"
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
    }

    void WolfCaster::render()
    {
        AIKO_FUNCTION_PROFILE

        constexpr float MapScaler = 512.0f;

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
            const aiko::vec2 tileSize = { MapScaler / m_wolfMap->getWidth(), MapScaler / m_wolfMap->getHeight()  };
            const aiko::vec2 playerScreenPosition = m_player.position * tileSize;
            const aiko::vec2 playerSize = tileSize; // / 2.0f;
            const aiko::vec2 playerPositionOffset = playerScreenPosition - playerSize * 0.5f;
            ui().rect(playerPositionOffset, playerSize, aiko::RED);

            const aiko::vec2 direction = aiko::vec2( aiko::math::cos(m_player.angleRadians), aiko::math::sin(m_player.angleRadians) );
            const aiko::vec2 forwardLength = tileSize * 3.0f;
            ui().line(playerScreenPosition, playerScreenPosition + ( direction * forwardLength), 1.0f, aiko::MAGENTA);

        }

    }

}

