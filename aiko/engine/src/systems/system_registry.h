#pragma once

#include <type_traits>
#include <utility>

#include "aiko_types.h"
#include "systems/sytem.h"

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
