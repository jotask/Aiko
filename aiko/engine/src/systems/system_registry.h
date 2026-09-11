#pragma once

#include "aiko_types.h"
#include "systems/system.h"

#include <type_traits>
#include <utility>

namespace aiko
{
    class SystemRegistry
    {
    public:
        using Systems = vector<AikoUPtr<System>>;

        explicit SystemRegistry(Systems& systems)
            : m_systems(systems)
        {
        }

        template<typename TSystem, typename... Args>
        TSystem* add(Args&&... args)
        {
            static_assert(std::is_base_of_v<System, TSystem>, "SystemRegistry::add requires a System type");

            auto instance = std::make_unique<TSystem>(
                std::forward<Args>(args)...
            );

            TSystem* raw = instance.get();
            m_systems.emplace_back(std::move(instance));
            return raw;
        }

    private:
        Systems& m_systems;
    };
}
