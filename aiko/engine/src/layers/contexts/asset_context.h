#pragma once

#include <aiko_types.h>
#include <assets/asset_id.h>

namespace aiko
{

    class SystemConnector;
    class AssetSystem;
    class Font;
    struct MeshAsset;

    class AssetContext
    {
    public:
        AssetId loadShader(string_view source);
        AssetId loadShader(string_view vertexSource, string_view fragmentSource);
        AssetId loadTexture(string_view source);
        AssetId createMesh(const MeshAsset& mesh);
        Font loadFont(string_view source, float pixelSize = 48.0f);
    private:
        friend class LayerContext;

        explicit AssetContext(SystemConnector& connector);

        AssetSystem* m_assetSystem = nullptr;
    };
}
