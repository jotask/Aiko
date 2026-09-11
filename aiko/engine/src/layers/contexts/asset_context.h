#pragma once

#include <aiko_types.h>
#include <assets/asset_id.h>

namespace aiko
{

    class SystemConnector;
    class AssetSystem;

    class AssetContext
    {
    public:
        AssetId loadShader(string_view source);
    private:
        friend class LayerContext;

        explicit AssetContext(SystemConnector& connector);

        AssetSystem* m_assetSystem = nullptr;
    };
}
