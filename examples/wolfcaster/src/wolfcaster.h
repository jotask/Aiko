#pragma once

#include "audio/wolf_audio_player.h"
#include "map/wolf_map.h"
#include "objects/wolf_player.h"
#include "wolfcaster_types.h"

#include <layers/layer.h>

#include <aiko_includes.h>

namespace wc
{

    class WolfCaster : public aiko::Layer
    {
    public:
        WolfCaster();
        ~WolfCaster();

    protected:
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;
        
    private:

        void loadLevel(uint16_t levelIndex);

        void renderLevelSelector();
        void renderMiniMap();
        void renderAllWallTextures();
        void renderAllSpriteTextures();
        void renderAllHudTexture();
        void renderAudioPlayer();

        std::vector<WolfLevelInfo> m_availableLevels;
        std::optional<uint16_t> m_requestedLevel;

        WolfMap m_wolfMap;

        aiko::AssetId m_mapTextureId = aiko::InvalidAssetId;

        std::vector<aiko::AssetId> m_wallTextures;
        std::vector<aiko::AssetId> m_spriteTextures;
        std::vector<aiko::AssetId> m_hudTextures;

        WolfPlayer m_player;

        WolfAudioPlayer m_soundPlayer;

        WolfCasterDebug m_debugImgui;

    };

}

