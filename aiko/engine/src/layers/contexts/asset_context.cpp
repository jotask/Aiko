#include "asset_context.h"

#include "systems/system_connector.h"
#include "systems/asset_system.h"

namespace aiko
{

    AssetContext::AssetContext(SystemConnector& connector)
    {
        m_assetSystem = connector.find<AssetSystem>();
        AIKO_ASSERT(m_assetSystem != nullptr, "Required system AssetSystem not found");
    }

    AssetId AssetContext::loadShader(string_view source)
    {
        const AssetId id = m_assetSystem->registerAsset<ShaderAsset>(source);
        m_assetSystem->loadAsset<ShaderAsset>(id);
        return id;
    }

}
