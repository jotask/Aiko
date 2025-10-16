#pragma once

#include "types/asset_type.h"
#include <vector>
#include <algorithm>

namespace aiko
{
    namespace asset
    {
        class System;
        class AssetConnector
        {
        public:
            AssetConnector(std::vector<System*>);

            template<class T>
            T* findSystem();

        private:
            std::vector<System*> m_systems;
        };

        template<class T>
        inline T* AssetConnector::findSystem()
        {
            auto it = std::find_if(m_systems.begin(), m_systems.end(), [](System* module) {
                return dynamic_cast<T*>(module) != nullptr;
            });
            return (it != m_systems.end()) ? dynamic_cast<T*>(*it) : nullptr;
        }

    }
}
