#pragma once

namespace wc
{

    inline bool GodMode = false;
    inline bool UnlimitedAmmo = false;

    struct WolfCasterDebug
    {
        bool renderGameSettings = false;
        bool renderMiniMap = false;
        bool renderWallTextures = false;
        bool renderSpriteTextures = false;
        bool renderHudTextures = false;
        bool renderSoundPlayer = false;
    };

}