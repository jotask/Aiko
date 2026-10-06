#pragma once

#include <aiko_types.h>
#include <assets/asset_id.h>

namespace aiko
{

    class SystemConnector;
    class AssetSystem;
    class Font;
    struct MeshAsset;
    struct TextureAsset;

    class AssetContext
    {
    public:
        AssetId loadShader(string_view source);
        AssetId loadShader(string_view vertexSource, string_view fragmentSource);

        AssetId createMesh(const MeshAsset& mesh);

        AssetId loadTexture(string_view source);
        AssetId createTexture(const TextureAsset& texture);
        TextureAsset& getMutableTexture(const AssetId& textureId);
        void invalidateTexture(const AssetId& textureId);

        Font loadFont(string_view source, float pixelSize = 48.0f);
    private:
        friend class LayerContext;

        explicit AssetContext(SystemConnector& connector);

        AssetSystem* m_assetSystem = nullptr;
    };
}
