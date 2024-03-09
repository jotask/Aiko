#pragma once

#include <memory>
#include <vector>

#include "aiko_types.h"
#include "systems/sytem.h"

namespace aiko
{

#define BIND_SYSTEM_REQUIRED(SystemType, Connector, VariableName) \
    VariableName = Connector->find<SystemType>(); \
    if (VariableName == nullptr) { \
        throw std::runtime_error("Required system " #SystemType " not found"); \
    }

#define BIND_SYSTEM_OPTIONAL(SystemType, Connector, VariableName) \
    VariableName = Connector->find<SystemType>(); \

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
            return (it != m_systems.end()) ? dynamic_cast<T*>(it->get()) : nullptr;
        }
    
    private:
        Systems& m_systems;
    };

}
