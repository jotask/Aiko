#pragma once

#include <aiko_types.h>
#include <assets/asset_id.h>

namespace aiko
{

    class SystemConnector;
    class AssetSystem;
    class Font;

    class AssetContext
    {
    public:
        AssetId loadShader(string_view source);
        AssetId loadTexture(string_view source);
        Font loadFont(string_view source, float pixelSize = 48.0f);
    private:
        friend class LayerContext;

        explicit AssetContext(SystemConnector& connector);

        AssetSystem* m_assetSystem = nullptr;
    };
}
