#pragma once

#include "aiko_types.h"
#include "systems/system.h"

#include <algorithm>
#include <memory>
#include <type_traits>

namespace aiko
{

    #define BIND_SYSTEM_REQUIRED(SystemType, Connector, VariableName) \
    VariableName = Connector->find<SystemType>(); \
    AIKO_ASSERT(VariableName != nullptr, "Required system " #SystemType " not found");

    #define BIND_SYSTEM_OPTIONAL(SystemType, Connector, VariableName) \
    VariableName = Connector->find<SystemType>(); \

    #define BIND_SYSTEM_REQUIRED_REF(SystemType, Connector, VariableName) \
    VariableName = Connector.find<SystemType>(); \
    AIKO_ASSERT(VariableName != nullptr, "Required system " #SystemType " not found")

    #define BIND_SYSTEM_OPTIONAL_REF(SystemType, Connector, VariableName) \
    VariableName = Connector.find<SystemType>()

    class SystemConnector
    {
    public:
    
        using Systems = vector<AikoUPtr<System>>;
    
        SystemConnector(Systems& systems)
            : m_systems(systems)
        {
        };
        ~SystemConnector() = default;

        template<class T>
        T* find()
        {
            static_assert(std::is_base_of_v<System, T>, "SystemConnector::find requires a System type");
            auto it = std::find_if(m_systems.begin(), m_systems.end(), [](const aiko::AikoUPtr<System>& system) {
                return dynamic_cast<T*>(system.get()) != nullptr;
            });
            return (it != m_systems.end()) ? dynamic_cast<T*>(it->get()) : nullptr;
        }
    
    private:
        Systems& m_systems;
    };

}
