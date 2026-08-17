#include "vulkan_mesh_impl.h"

#include "models/mesh.h"
#include "platform/vulkan/vulkan_types.h"

namespace aiko::renderer::vulkan
{
    namespace shared {
        struct VertexInformation;
    }

    VulkanMeshImpl::VulkanMeshImpl(const Mesh* mesh)
        : IMeshImpl(mesh)
    {

    }

    bool VulkanMeshImpl::isValid() const
    {
        return false;
    }

    void VulkanMeshImpl::unload()
    {

    }

    void VulkanMeshImpl::refresh(const MeshAsset& asset)
    {

    }

    uint VulkanMeshImpl::id()
    {
        return {};
    }

}
