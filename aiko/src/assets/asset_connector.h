#pragma once

#include "types/asset_type.h"
#include <vector>

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
            auto it = std::find_if(m_modules.begin(), m_modules.end(), [](const aiko::AikoUPtr<Module>& module) {
                return dynamic_cast<T*>(module.get()) != nullptr;
                });
            return (it != m_modules.end()) ? dynamic_cast<T*>(it->get()) : nullptr;
        }

    }
}
