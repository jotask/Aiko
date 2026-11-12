#include "wolfcaster.h"

#include "assets/wolf_assets_decoder.h"
#include "assets/wolf_map_texture.h"
#include "assets/wolf_vga_graphics_reader.h"
#include "assets/wolf_vga_graphics_types.h"
#include "assets/wolf_vswap_reader.h"
#include "core/file.h"
#include "layers/contexts/asset_context.h"
#include "layers/contexts/input_context.h"
#include "layers/contexts/render_context.h"
#include "layers/contexts/scene_context.h"
#include "layers/contexts/ui_context.h"
#include "map/wolf_map_loader.h"
#include "raycasting/wolf_raycast_renderer.h"
#include "raycasting/wolf_raycaster.h"
#include "systems/wolf_combat_system.h"
#include "systems/wolf_pickup_system.h"

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

    namespace
    {
        constexpr float MaxInteractionDistance = 1.5f;
    }

    WolfCaster::WolfCaster()
        : m_wolfMap(WolfMapLoader::loadLevel(WOLF3D_DATA_DIR, 0))
    {

    }

    WolfCaster::~WolfCaster()
    {
        m_soundPlayer.shutdown();
    }

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

        m_availableLevels = WolfMapLoader::getAvailableLevels(dataDir);

        const WolfVSwapData assetData = WolfVSwapReader::load(dataDir / "VSWAP.WL1");
        for (const WolfWallTexture& wallTexture : assetData.walls)
        {
            const aiko::TextureAsset asset = WolfAssetsDecoder::decode(wallTexture);
            const aiko::AssetId assetId = assets().createTexture(asset);
            m_wallTextures.push_back(assetId);
        }
        for (const WolfSprite& spriteTexture : assetData.sprites)
        {
            const aiko::TextureAsset asset = WolfAssetsDecoder::decode(spriteTexture);
            const aiko::AssetId assetId = assets().createTexture(asset);
            m_spriteTextures.push_back(assetId);
        }

        const aiko::TextureAsset asset = WolfMapTexture::build(m_wolfMap);
        m_mapTextureId = assets().createTexture(asset);

        const WolfPlayerSpawn& playerSpawn = m_wolfMap.getPlayerSpawn();
        aiko::logger::Log::infoFmt("WolfCaster Player Spawn");
        aiko::logger::Log::infoFmt("     Tile: ({} , {})", playerSpawn.tile.x, playerSpawn.tile.y);
        aiko::logger::Log::infoFmt("     Direction: {}", magic_enum::enum_name(playerSpawn.direction));

        m_player = WolfPlayer::fromSpawn(playerSpawn);

        m_soundPlayer.init(std::move(assetData.sounds));

        const WolfVgaGraphicsData vgaData = WolfVgaGraphicsReader::load(WOLF3D_DATA_DIR);
        const auto pictureDimensions = WolfVgaGraphicsReader::decodePictureDimensions(vgaData, 144);
        AIKO_ASSERT(pictureDimensions.size() == 144, "Unexpected VGA picture count");

        for (size_t i = 0 ; i < pictureDimensions.size(); ++i)
        {
            const WolfVgaPictureDimension& dimension = pictureDimensions[i];
            WolfVgaPicture picture = WolfVgaGraphicsReader::decodePicture(vgaData, i + 3, dimension);
            AIKO_ASSERT(picture.paletteIndices.size() == size_t(picture.width) * picture.height, "VGA picture palette index count mismatch");
            const aiko::TextureAsset pictureAsset = WolfAssetsDecoder::decode(picture);
            const aiko::AssetId assetId = assets().createTexture(pictureAsset);
            m_hudTextures.push_back(assetId);
        }

        aiko::logger::Log::infoFmt("VGA picture dimensions decoded: {} pictures", pictureDimensions.size());

    }

    void WolfCaster::update()
    {
        AIKO_FUNCTION_PROFILE

        if (m_requestedLevel.has_value())
        {
            const uint16_t levelIndex = *m_requestedLevel;
            m_requestedLevel.reset();
            loadLevel(levelIndex);
            return;
        }

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
            m_player.move(displacement, m_wolfMap);
        }


        if (input().isKeyJustPressed(aiko::Key::KEY_F))
        {
            const WolfRayHit hit = WolfRaycaster::castRay(m_wolfMap, m_player.position(), m_player.forward(), WolfRaycastMode::Interaction);
            if (hit.hit && hit.distance < MaxInteractionDistance)
            {
                if (m_wolfMap.isExitSwitch(hit.tileX, hit.tileY))
                {
                    completeLevel();
                }
                else if (m_wolfMap.canInteract(hit.tileX, hit.tileY))
                {
                    const aiko::vec2 forward = m_player.forward();
                    aiko::ivec2 pushDirection = {0};
                    if (hit.side == WolfRaySide::Vertical)
                    {
                        pushDirection.x = forward.x >= 0.0f ? 1 : -1;
                    }
                    else
                    {
                        pushDirection.y = forward.y >= 0.0f ? 1 : -1;
                    }
                    const uint8_t ownedKeys = DisableKeys ? 0b00001111 : m_player.keys();
                    m_wolfMap.interact(hit.tileX, hit.tileY, pushDirection, m_player.position(), ownedKeys);
                }
            }
        }

        m_player.updateWeapon(deltaTime, input().isKeyPressed(aiko::Key::KEY_SPACE), input().isKeyJustPressed(aiko::Key::KEY_SPACE));

        m_wolfMap.update(deltaTime, m_player.position());

        WolfPickupSystem::updateMapPickups(m_wolfMap, m_player);

        WolfCombatSystem::update(m_wolfMap, m_player);

    }

    void WolfCaster::render()
    {
        AIKO_FUNCTION_PROFILE

        WolfRaycastRenderer::render(m_wolfMap, m_player, m_wallTextures, m_spriteTextures, m_hudTextures, ui(), aiko::vec2(renderer().getRenderSize()));

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Tools"))
            {
                ImGui::MenuItem("Render Game Settings", nullptr, &m_debugImgui.renderGameSettings);
                ImGui::MenuItem("Render MiniMap", nullptr, &m_debugImgui.renderMiniMap);
                ImGui::MenuItem("Render Wall Textures", nullptr, &m_debugImgui.renderWallTextures);
                ImGui::MenuItem("Render Sprite Textures", nullptr, &m_debugImgui.renderSpriteTextures);
                ImGui::MenuItem("Render HUD Textures", nullptr, &m_debugImgui.renderHudTextures);
                ImGui::MenuItem("Render Sound Player", nullptr, &m_debugImgui.renderSoundPlayer);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        renderGameSettings();
        renderMiniMap();
        renderAllWallTextures();
        renderAllSpriteTextures();
        renderAllHudTexture();
        renderAudioPlayer();

    }

    void WolfCaster::loadLevel(uint16_t levelIndex)
    {
        m_wolfMap = WolfMapLoader::loadLevel(WOLF3D_DATA_DIR, levelIndex);
        m_player = WolfPlayer::fromSpawn(m_wolfMap.getPlayerSpawn());

        assets().getMutableTexture(m_mapTextureId) = WolfMapTexture::build(m_wolfMap);
        assets().invalidateTexture(m_mapTextureId);

    }

    void WolfCaster::completeLevel()
    {
        constexpr std::array<uint16_t, 6> ElevatorBackTo = {1, 1, 7, 3, 5, 3};
        constexpr uint16_t LevelsPerEpisode = 10;
        constexpr uint16_t FinalFloorIndex = 8;
        constexpr uint16_t SecretFloorIndex = 9;
        constexpr TileId SecretExitTile = 107;

        const uint16_t currentLevel = m_wolfMap.getLevelIndex();
        const uint16_t episodeIndex = currentLevel / LevelsPerEpisode;
        const uint16_t episodeBase = episodeIndex * LevelsPerEpisode;
        const uint16_t floorIndex = currentLevel % LevelsPerEpisode;

        AIKO_ASSERT(episodeIndex < ElevatorBackTo.size(), "Invalid episode index");

        if (floorIndex == FinalFloorIndex)
        {
            aiko::logger::Log::info("Episode completed");
            return;
        }

        const aiko::ivec2 playerTile = {
            static_cast<int32_t>(std::floor(m_player.position().x)),
            static_cast<int32_t>(std::floor(m_player.position().y))
        };
        AIKO_ASSERT(playerTile.x >= 0 && playerTile.x < m_wolfMap.getWidth() && playerTile.y >= 0 && playerTile.y < m_wolfMap.getHeight(), "Player position outside map bounds");

        const TileId playerTileId = m_wolfMap.getGeometryTile(playerTile.x, playerTile.y);
        uint16_t nextLevel = currentLevel + 1;

        if (floorIndex == SecretFloorIndex)
        {
            // Return from the secret level.
            nextLevel = episodeBase + ElevatorBackTo[episodeIndex];
        }
        else if (playerTileId == SecretExitTile)
        {
            // Enter the episode's secret level.
            nextLevel = episodeBase + SecretFloorIndex;
        }

        const auto found = std::find_if(
            m_availableLevels.begin(),
            m_availableLevels.end(),
            [nextLevel](const WolfLevelInfo& level)
            {
                return level.index == nextLevel;
            }
        );

        if (found == m_availableLevels.end())
        {
            aiko::logger::Log::infoFmt("Level {} is not available", nextLevel + 1);
            return;
        }
        aiko::logger::Log::infoFmt("Level completed: {} -> {}", currentLevel + 1, nextLevel + 1);
        m_requestedLevel = nextLevel;
    }

    void WolfCaster::renderGameSettings()
    {
        if (m_debugImgui.renderGameSettings == false)
        {
            return;
        }

        if (ImGui::Begin("Game Settings", &m_debugImgui.renderGameSettings))
        {

            ImGui::Checkbox("God Mode", &GodMode);
            ImGui::Checkbox("Unlimited ammo", &UnlimitedAmmo);
            ImGui::Checkbox("Ignore keys requirements", &DisableKeys);

            const uint16_t currentLevel = m_wolfMap.getLevelIndex();

            ImGui::Text("Current Level: %u", unsigned(currentLevel + 1));
            ImGui::Text("Name: %s", m_wolfMap.getName().c_str());
            ImGui::Text("Levels Available: %zu", m_availableLevels.size());

            if (ImGui::Button("Complete Level"))
            {
                completeLevel();
            }

            ImGui::Separator();

            const std::string_view currentWeapon = magic_enum::enum_name(m_player.getWeapon());
            ImGui::Text("Current Weapon selected: %s", currentWeapon.data());
            if (ImGui::BeginCombo("Weapons", currentWeapon.data()))
            {
                for (const auto& [weapon, weaponName] : magic_enum::enum_entries<WolfWeapon>())
                {
                    const bool selected = weaponName == currentWeapon;
                    if (ImGui::Selectable(weaponName.data(), selected))
                    {
                        m_player.setWeapon(weapon);
                    }
                    if (selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            int32_t ammo = m_player.ammo();
            if (ImGui::SliderInt("Ammo", &ammo, 0, WolfPlayer::MaxAmmo))
            {
                m_player.setAmmo(ammo);
            }

            ImGui::Separator();

            ImGuiStorage* storage = ImGui::GetStateStorage();
            const ImGuiID selectionId = ImGui::GetID("SelectedLevel");

            const int selectedLevel = storage->GetInt(selectionId, currentLevel);

            const std::string preview = std::format("Level {}", selectedLevel + 1);

            if (ImGui::BeginCombo("Levels", preview.c_str()))
            {
                for (const WolfLevelInfo& level : m_availableLevels)
                {
                    const bool selected = selectedLevel == level.index;
                    const std::string label = std::format("{} - {} ({}x{})", level.index + 1, level.name, level.width, level.height);
                    if (ImGui::Selectable(label.c_str(), selected))
                    {
                        storage->SetInt(selectionId, level.index);
                    }

                    if (selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }

                ImGui::EndCombo();
            }

            ImGui::BeginDisabled(m_availableLevels.empty());

            if (ImGui::Button("Load Selected Level"))
            {
                m_requestedLevel = static_cast<uint16_t>(storage->GetInt(selectionId, currentLevel));
            }

            ImGui::EndDisabled();

            ImGui::SameLine();

            if (ImGui::Button("Restart Level"))
            {
                m_requestedLevel = currentLevel;
            }

            if (m_requestedLevel.has_value())
            {
                ImGui::TextDisabled("Level change queued");
            }
        }

        ImGui::End();
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
                const float mapWidth = static_cast<float>(m_wolfMap.getWidth());
                const float mapHeight = static_cast<float>(m_wolfMap.getHeight());

                const float scale = std::min( available.x / mapWidth, available.y / mapHeight);
                const ImVec2 imageSize = { mapWidth * scale, mapHeight * scale };

                const ImVec2 cursor = ImGui::GetCursorPos();
                ImGui::SetCursorPos({ cursor.x + (available.x - imageSize.x) * 0.5f, cursor.y + (available.y - imageSize.y) * 0.5f});
                const ImTextureID textureId = static_cast<ImTextureID>(renderer().getTextureId(m_mapTextureId, sampler));
                ImGui::Image(textureId, imageSize);

                // Get the actual position of the minimap image.
                const ImVec2 imagePosition = ImGui::GetItemRectMin();

                const float scaleX = imageSize.x / static_cast<float>(m_wolfMap.getWidth());
                const float scaleY = imageSize.y / static_cast<float>(m_wolfMap.getHeight());

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


    void WolfCaster::renderAllWallTextures()
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

            for (size_t i = 0; i < m_wallTextures.size(); ++i)
            {
                const aiko::AssetId assetId = m_wallTextures[i];

                if (assetId == aiko::InvalidAssetId)
                {
                    continue;
                }

                const ImTextureID textureId = static_cast<ImTextureID>(renderer().getTextureId(assetId, sampler));

                ImGui::BeginGroup();

                ImGui::Text("Wall %zu", i);
                ImGui::Image(textureId, ImVec2(textureSize, textureSize));

                ImGui::EndGroup();

                if ((i + 1) % columns != 0 && i + 1 < m_wallTextures.size())
                {
                    ImGui::SameLine();
                }
            }
        }

        ImGui::End();
    }

    void WolfCaster::renderAllSpriteTextures()
    {
        if (m_debugImgui.renderSpriteTextures == false)
        {
            return;
        }

        if (ImGui::Begin("Sprite Textures", &m_debugImgui.renderSpriteTextures))
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

            for (size_t i = 0; i < m_spriteTextures.size(); ++i)
            {
                const aiko::AssetId assetId = m_spriteTextures[i];

                if (assetId == aiko::InvalidAssetId)
                {
                    continue;
                }

                const ImTextureID textureId = static_cast<ImTextureID>(renderer().getTextureId(assetId, sampler));

                ImGui::BeginGroup();

                ImGui::Text("Sprite %zu", i);
                ImGui::Image(textureId, ImVec2(textureSize, textureSize));

                ImGui::EndGroup();

                if ((i + 1) % columns != 0 && i + 1 < m_spriteTextures.size())
                {
                    ImGui::SameLine();
                }
            }
        }

        ImGui::End();

    }

    void WolfCaster::renderAllHudTexture()
    {
        if (m_debugImgui.renderHudTextures == false)
        {
            return;
        }

        if (ImGui::Begin("HUD Textures", &m_debugImgui.renderHudTextures))
        {
            const aiko::SamplerState sampler = {
                .minFilter = aiko::TextureFilter::Nearest,
                .magFilter = aiko::TextureFilter::Nearest,
                .mipFilter = aiko::TextureMipFilter::None,
                .wrapU = aiko::TextureWrapMode::Clamp,
                .wrapV = aiko::TextureWrapMode::Clamp,
            };

            constexpr float cellWidth = 340.0f;

            const float availableWidth = ImGui::GetContentRegionAvail().x;
            const int columnCount = std::max(1, static_cast<int>(availableWidth / cellWidth));

            if (ImGui::BeginTable("HudTexturesTable", columnCount, ImGuiTableFlags_SizingStretchSame))
            {
                for (size_t i = 0; i < m_hudTextures.size(); ++i)
                {
                    const aiko::AssetId assetId = m_hudTextures[i];

                    if (assetId == aiko::InvalidAssetId)
                    {
                        continue;
                    }

                    const aiko::TextureAsset& texture = assets().getMutableTexture(assetId);
                    const ImTextureID textureId = static_cast<ImTextureID>(renderer().getTextureId(assetId, sampler));
                    const ImVec2 imageSize = {static_cast<float>(texture.desc.width), static_cast<float>(texture.desc.height)};

                    ImGui::TableNextColumn();
                    ImGui::Text("Picture %zu (Chunk %zu) - %dx%d", i, i + 3, texture.desc.width, texture.desc.height);
                    ImGui::Image(textureId, imageSize);
                }

                ImGui::EndTable();
            }
        }

        ImGui::End();
    }

    void WolfCaster::renderAudioPlayer()
    {
        if (m_debugImgui.renderSoundPlayer == false)
        {
            return;
        }
        if (ImGui::Begin("Sound Player", &m_debugImgui.renderSoundPlayer))
        {
            static size_t currentSoundIndex = 0;
            const size_t soundCount = m_soundPlayer.getSoundsSize();

            ImGui::Text("Sounds: %zu", m_soundPlayer.getSoundsSize());
            ImGui::Text("Current selected audio: %zu", currentSoundIndex);

            if (ImGui::Button("<"))
            {
                currentSoundIndex = (currentSoundIndex + soundCount - 1) % soundCount;
            }
            ImGui::SameLine();
            if (ImGui::Button("Play"))
            {
                m_soundPlayer.play(currentSoundIndex);
            }
            ImGui::SameLine();
            if (ImGui::Button(">"))
            {
                currentSoundIndex = (currentSoundIndex + 1) % soundCount;
            }

            if (ImGui::Button("Stop"))
            {
                m_soundPlayer.stop();
            }

        }
        ImGui::End();
    }

}

