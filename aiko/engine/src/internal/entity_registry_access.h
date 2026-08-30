#pragma once

#include "models/entity_id.h"

#include <entt/entt.hpp>

namespace aiko
{
    class EntityRegistry;

    class EntityRegistryAccess
    {
    public:
        static entt::registry& get(EntityRegistry& registry);
        static const entt::registry& get(const EntityRegistry& registry);

        static entt::entity toEntity(EntityId entity)
        {
            return static_cast<entt::entity>(entity.value);
        }

    };
}
