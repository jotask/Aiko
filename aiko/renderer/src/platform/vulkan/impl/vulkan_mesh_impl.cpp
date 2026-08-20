#include "vulkan_mesh_impl.h"

#include "models/mesh.h"
#include "platform/vulkan/vulkan_context.h"
#include "platform/vulkan/vulkan_types.h"

#include <cstring>

namespace aiko::renderer::vulkan
{

    namespace
    {

        vector<VulkanVertex> buildVertices(const MeshAsset& asset)
        {
            vector<VulkanVertex> vertices;
            vertices.reserve(asset.m_vertices.size());

            for (size_t i = 0; i < asset.m_vertices.size(); ++i)
            {
                VulkanVertex v{};
                v.position = asset.m_vertices[i];
                v.normal = i < asset.m_normals.size() ? asset.m_normals[i] : vec3{0.0f, 0.0f, 1.0f};
                v.uv = i < asset.m_textCoord.size() ? asset.m_textCoord[i] : vec2{0.0f, 0.0f};
                v.color = i < asset.m_colors.size() ? asset.m_colors[i] : WHITE;
                vertices.push_back(v);
            }

            return vertices;
        }
    }


    VulkanMeshImpl::VulkanMeshImpl(const Mesh* mesh)
        : IMeshImpl(mesh)
    {

    }

    bool VulkanMeshImpl::isValid() const
    {
        return m_vertexBuffer != VK_NULL_HANDLE &&
           m_vertexMemory != VK_NULL_HANDLE &&
           m_indexBuffer != VK_NULL_HANDLE &&
           m_indexMemory != VK_NULL_HANDLE &&
           m_indexCount > 0;
    }

    void VulkanMeshImpl::unload()
    {
        if (m_vertexBuffer == VK_NULL_HANDLE &&
        m_indexBuffer == VK_NULL_HANDLE)
        {
            m_indexCount = 0;
            return;
        }

        VulkanContext& ctx = VulkanContext::current();
        VkDevice device = ctx.device();

        if (m_vertexBuffer != VK_NULL_HANDLE) vkDestroyBuffer(device, m_vertexBuffer, nullptr);
        if (m_vertexMemory != VK_NULL_HANDLE) vkFreeMemory(device, m_vertexMemory, nullptr);
        if (m_indexBuffer != VK_NULL_HANDLE) vkDestroyBuffer(device, m_indexBuffer, nullptr);
        if (m_indexMemory != VK_NULL_HANDLE) vkFreeMemory(device, m_indexMemory, nullptr);

        m_vertexBuffer = VK_NULL_HANDLE;
        m_vertexMemory = VK_NULL_HANDLE;
        m_indexBuffer = VK_NULL_HANDLE;
        m_indexMemory = VK_NULL_HANDLE;
        m_indexCount = 0;
    }

    void VulkanMeshImpl::refresh(const MeshAsset& asset)
    {
        unload();

        AIKO_ASSERT(!asset.m_vertices.empty(), "Mesh has no vertices");
        AIKO_ASSERT(!asset.m_indices.empty(), "Mesh has no indices");

        VulkanContext& ctx = VulkanContext::current();
        VkDevice device = ctx.device();

        const vector<VulkanVertex> vertices = buildVertices(asset);
        const VkDeviceSize vertexSize = sizeof(VulkanVertex) * vertices.size();

        VkBuffer stagingVertexBuffer = VK_NULL_HANDLE;
        VkDeviceMemory stagingVertexMemory = VK_NULL_HANDLE;

        ctx.createBuffer(vertexSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingVertexBuffer,
            stagingVertexMemory);

        void* data = nullptr;
        vkMapMemory(device, stagingVertexMemory, 0, vertexSize, 0, &data);
        std::memcpy(data, vertices.data(), static_cast<size_t>(vertexSize));
        vkUnmapMemory(device, stagingVertexMemory);

        ctx.createBuffer(vertexSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            m_vertexBuffer,
            m_vertexMemory);

        ctx.copyBuffer(stagingVertexBuffer, m_vertexBuffer, vertexSize);

        vkDestroyBuffer(device, stagingVertexBuffer, nullptr);
        vkFreeMemory(device, stagingVertexMemory, nullptr);

        m_indexCount = static_cast<u32>(asset.m_indices.size());
        const VkDeviceSize indexSize = sizeof(uint16_t) * asset.m_indices.size();

        VkBuffer stagingIndexBuffer = VK_NULL_HANDLE;
        VkDeviceMemory stagingIndexMemory = VK_NULL_HANDLE;

        ctx.createBuffer(indexSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingIndexBuffer,
            stagingIndexMemory);

        vkMapMemory(device, stagingIndexMemory, 0, indexSize, 0, &data);
        std::memcpy(data, asset.m_indices.data(), static_cast<size_t>(indexSize));
        vkUnmapMemory(device, stagingIndexMemory);

        ctx.createBuffer(indexSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            m_indexBuffer,
            m_indexMemory);

        ctx.copyBuffer(stagingIndexBuffer, m_indexBuffer, indexSize);

        vkDestroyBuffer(device, stagingIndexBuffer, nullptr);
        vkFreeMemory(device, stagingIndexMemory, nullptr);
    }

    uint VulkanMeshImpl::id()
    {
        return static_cast<uint>(reinterpret_cast<uintptr_t>(m_vertexBuffer));
    }

}
