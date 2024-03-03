#pragma once

#include <memory>
#include <vector>

#include "aiko_types.h"
#include "systems/sytem.h"

namespace aiko
{

#define BIND_SYSTEM_REQUIRED(ModuleType, Connector, VariableName) \
    VariableName = Connector->find<ModuleType>(); \
    if (VariableName == nullptr) { \
        throw std::runtime_error("Required module " #ModuleType " not found"); \
    }

#define BIND_SYSTEM_OPTIONAL(ModuleType, Connector, VariableName) \
    VariableName = Connector->find<ModuleType>(); \

    class SystemConnector
    {
    public:
    
        using Systems = std::vector<aiko::AikoUPtr<System>>;
    
        SystemConnector(Systems& systems)
            : m_systems(systems)
        {
        };
        ~SystemConnector() = default;

        template<class T>
        T* find()
        {
            auto it = std::find_if(m_systems.begin(), m_systems.end(), [](const aiko::AikoUPtr<System>& system) {
                return dynamic_cast<T*>(system.get()) != nullptr;
            });
            return (it != m_systems.end()) ? std::dynamic_pointer_cast<T>(*it) : nullptr;
        }
    
    private:
        Systems& m_systems;
    };

}
