#pragma once

#include <entt/entt.hpp>

namespace aiko
{
    class EntityRegistry;

    class EntityRegistryAccess
    {
    public:
        static entt::registry& get(EntityRegistry& registry);
        static const entt::registry& get(const EntityRegistry& registry);
    };
}
