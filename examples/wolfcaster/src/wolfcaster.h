#pragma once

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

        void renderBackground();
        void renderMiniMap();
        void renderRaycaster();
        void renderAllWallTexture();

        std::optional<WolfMap> m_wolfMap;

        aiko::AssetId m_mapTextureId = aiko::InvalidAssetId;

        std::vector<aiko::AssetId> m_wallsTexture;

        WolfPlayer m_player;

        WolfCasterDebug m_debugImgui;

    };

}

