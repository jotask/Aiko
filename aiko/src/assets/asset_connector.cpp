#pragma once

#include "types/asset_type.h"
#include "asset_connector.h"

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
