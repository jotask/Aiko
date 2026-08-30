#include "entity_registry.h"

#include <entt/entt.hpp>

namespace aiko
{
    struct EntityRegistry::Impl
    {
        entt::registry registry;
    };

    EntityRegistry::EntityRegistry()
        : m_impl(std::make_unique<Impl>())
    {
    }

    EntityRegistry::~EntityRegistry() = default;

    EntityId EntityRegistry::create()
    {
        const entt::entity entity = m_impl->registry.create();

        return EntityId
        {
            .value = static_cast<u32>(entt::to_integral(entity))
        };
    }

    void EntityRegistry::destroy(EntityId entity)
    {
        if (!valid(entity))
        {
            return;
        }

        m_impl->registry.destroy(static_cast<entt::entity>(entity.value));
    }

    bool EntityRegistry::valid(EntityId entity) const
    {
        if (!entity.valid())
        {
            return false;
        }

        return m_impl->registry.valid(static_cast<entt::entity>(entity.value));
    }

}
