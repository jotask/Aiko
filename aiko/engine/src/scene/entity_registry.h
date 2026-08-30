#pragma once

#include <aiko_types.h>

#include "models/entity_id.h"

namespace aiko
{

    class EntityRegistry
    {
    public:
        EntityRegistry();
        ~EntityRegistry();

        EntityRegistry(const EntityRegistry&) = delete;
        EntityRegistry& operator=(const EntityRegistry&) = delete;

        EntityId create();
        void destroy(EntityId entity);
        bool valid(EntityId entity) const;

    private:
        struct Impl;
        AikoUPtr<Impl> m_impl;
    };
}
