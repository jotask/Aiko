#include "wolfcaster.h"

#include "assets/wolf_assets_decoder.h"
#include "assets/wolf_map_texture.h"
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
            if (hit.distance < MaxInteractionDistance)
            {
                if (hit.hit == true && m_wolfMap.canInteract(hit.tileX, hit.tileY))
                {
                    m_wolfMap.interact(hit.tileX, hit.tileY);
                }
            }
        }

        m_wolfMap.update(deltaTime);

    }

    void WolfCaster::render()
    {
        AIKO_FUNCTION_PROFILE

        WolfRaycastRenderer::render(m_wolfMap, m_player, m_wallTextures, m_spriteTextures, ui(), aiko::vec2(renderer().getRenderSize()));

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Tools"))
            {
                ImGui::MenuItem("Render Level Selector", nullptr, &m_debugImgui.renderLevelSelector);
                ImGui::MenuItem("Render MiniMap", nullptr, &m_debugImgui.renderMiniMap);
                ImGui::MenuItem("Render Wall Textures", nullptr, &m_debugImgui.renderWallTextures);
                ImGui::MenuItem("Render Sprite Textures", nullptr, &m_debugImgui.renderSpriteTextures);
                ImGui::MenuItem("Render Sound Player", nullptr, &m_debugImgui.renderSoundPlayer);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        renderLevelSelector();
        renderMiniMap();
        renderAllWallTextures();
        renderAllSpriteTextures();
        renderAudioPlayer();

    }

    void WolfCaster::loadLevel(uint16_t levelIndex)
    {
        m_wolfMap = WolfMapLoader::loadLevel(WOLF3D_DATA_DIR, levelIndex);
        m_player = WolfPlayer::fromSpawn(m_wolfMap.getPlayerSpawn());

        assets().getMutableTexture(m_mapTextureId) = WolfMapTexture::build(m_wolfMap);
        assets().invalidateTexture(m_mapTextureId);

    }


    void WolfCaster::renderLevelSelector()
    {
        if (m_debugImgui.renderLevelSelector == false)
        {
            return;
        }

        if (ImGui::Begin("Level Selector", &m_debugImgui.renderLevelSelector))
        {
            const uint16_t currentLevel = m_wolfMap.getLevelIndex();

            ImGui::Text("Current Level: %u", unsigned(currentLevel + 1));
            ImGui::Text("Name: %s", m_wolfMap.getName().c_str());
            ImGui::Text("Levels Available: %zu", m_availableLevels.size());

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

