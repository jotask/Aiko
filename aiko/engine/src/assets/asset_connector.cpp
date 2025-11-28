#include "asset_connector.h"

#include "types/asset_type.h"

namespace aiko
{
    namespace asset
    {

        aiko::asset::AssetConnector::AssetConnector(std::vector<System*> sys)
            : m_systems(sys)
        {

        }

    }
}
