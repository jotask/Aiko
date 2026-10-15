#include "vulkan_resource_id.h"

#include <atomic>
#include <cstdlib>

namespace aiko::renderer::vulkan
{
    RenderResourceId allocateRenderResourceId()
    {
        static std::atomic<RenderResourceId> nextId{1};

        const RenderResourceId id = nextId.fetch_add(1, std::memory_order_relaxed);

        if (id == InvalidRenderResourceId)
        {
            std::abort();
        }

        return id;
    }
}
