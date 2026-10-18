#include "vulkan_mesh_impl.h"

#include "models/mesh.h"
#include "platform/vulkan/vulkan_context.h"
#include "platform/vulkan/vulkan_types.h"

#include <assets/mesh_tangents.h>
#include <logger/logger.h>

#include <cstring>

namespace aiko::renderer::vulkan
{

    namespace
    {

        vector<VulkanVertex> buildVertices(const MeshAsset& asset)
        {
            vector<VulkanVertex> vertices;
            vertices.reserve(asset.m_vertices.size());

            vector<vec4> generatedTangents;

            if (asset.m_tangents.empty())
            {
                generatedTangents = mesh::calculateTangents(asset);
            }

            const vector<vec4>& tangents = asset.m_tangents.empty() ? generatedTangents : asset.m_tangents;

            for (size_t i = 0; i < asset.m_vertices.size(); ++i)
            {
                VulkanVertex v{};
                v.position = asset.m_vertices[i];
                v.normal = i < asset.m_normals.size() ? asset.m_normals[i] : vec3{0.0f, 0.0f, 1.0f};
                v.uv = i < asset.m_textCoord.size() ? asset.m_textCoord[i] : vec2{0.0f, 0.0f};
                v.color = i < asset.m_colors.size() ? asset.m_colors[i] : WHITE;
                v.tangent = tangents.empty() ? vec4{0.0f, 0.0f, 0.0f, 1.0f} : tangents[i];
                vertices.push_back(v);
            }

            return vertices;
        }
    }


    VulkanMeshImpl::VulkanMeshImpl(const Mesh* mesh)
        : IMeshImpl(mesh)
    {

    }

    VulkanMeshImpl::~VulkanMeshImpl()
    {
        AIKO_ASSERT(m_vertexBuffer == VK_NULL_HANDLE && m_vertexMemory == VK_NULL_HANDLE && m_indexBuffer == VK_NULL_HANDLE && m_indexMemory == VK_NULL_HANDLE, "VulkanMeshImpl destroyed without unload()");
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
        if (m_vertexBuffer == VK_NULL_HANDLE && m_vertexMemory == VK_NULL_HANDLE && m_indexBuffer == VK_NULL_HANDLE && m_indexMemory == VK_NULL_HANDLE)
        {
            m_indexCount = 0;
            return;
        }

        const VkBuffer vertexBuffer = m_vertexBuffer;
        const VkDeviceMemory vertexMemory = m_vertexMemory;
        const VkBuffer indexBuffer = m_indexBuffer;
        const VkDeviceMemory indexMemory = m_indexMemory;

        m_vertexBuffer = VK_NULL_HANDLE;
        m_vertexMemory = VK_NULL_HANDLE;
        m_indexBuffer = VK_NULL_HANDLE;
        m_indexMemory = VK_NULL_HANDLE;
        m_indexCount = 0;

        VulkanContext& ctx = VulkanContext::current();

        ctx.retireBuffer(vertexBuffer, vertexMemory);
        ctx.retireBuffer(indexBuffer, indexMemory);
    }

    void VulkanMeshImpl::refresh(const MeshAsset& asset)
    {
        AIKO_ASSERT(!asset.m_vertices.empty(), "Mesh has no vertices");
        AIKO_ASSERT(!asset.m_indices.empty(), "Mesh has no indices");
        AIKO_ASSERT(asset.m_tangents.empty() || asset.m_tangents.size() == asset.m_vertices.size(), "Mesh tangent count must match vertex count");

        VulkanContext& ctx = VulkanContext::current();
        const VkDevice device = ctx.device();

        const vector<VulkanVertex> vertices = buildVertices(asset);

        const VkDeviceSize vertexSize = sizeof(VulkanVertex) * vertices.size();
        const VkDeviceSize indexSize = sizeof(uint16_t) * asset.m_indices.size();

        struct BufferAllocation
        {
            VkBuffer buffer = VK_NULL_HANDLE;
            VkDeviceMemory memory = VK_NULL_HANDLE;
        };

        BufferAllocation vertexStaging;
        BufferAllocation indexStaging;
        BufferAllocation newVertex;
        BufferAllocation newIndex;

        const auto destroy = [device](BufferAllocation& allocation)
        {
            if (allocation.buffer != VK_NULL_HANDLE)
            {
                vkDestroyBuffer(device, allocation.buffer, nullptr);
            }

            if (allocation.memory != VK_NULL_HANDLE)
            {
                vkFreeMemory(device, allocation.memory, nullptr);
            }

            allocation = {};
        };

        const auto cleanup = [&]()
        {
            destroy(vertexStaging);
            destroy(indexStaging);
            destroy(newVertex);
            destroy(newIndex);
        };

        const auto allocate = [&](BufferAllocation& allocation, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, const char* label) -> bool
        {
            const VkResult result = ctx.createBuffer(size, usage, properties, allocation.buffer, allocation.memory);

            if (result != VK_SUCCESS)
            {
                logger::Log::error("Failed to create Vulkan mesh %s buffer (VkResult %d)", label, static_cast<int>(result));
                return false;
            }

            return true;
        };

        constexpr VkMemoryPropertyFlags stagingProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        constexpr VkMemoryPropertyFlags deviceProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        // Allocate all resources before modifying the current mesh.
        if (!allocate(vertexStaging, vertexSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingProperties, "vertex staging") ||
            !allocate(newVertex, vertexSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, deviceProperties, "vertex") ||
            !allocate(indexStaging, indexSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingProperties, "index staging") ||
            !allocate(newIndex, indexSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, deviceProperties, "index"))
        {
            cleanup();
            return;
        }

        // Upload vertex data into staging memory.
        void* mapped = nullptr;

        VkResult mapResult = vkMapMemory(device, vertexStaging.memory, 0, vertexSize, 0, &mapped);

        if (mapResult != VK_SUCCESS)
        {
            logger::Log::error("Failed to map Vulkan vertex staging memory (VkResult %d)", static_cast<int>(mapResult));
            cleanup();
            return;
        }

        std::memcpy(mapped, vertices.data(), static_cast<size_t>(vertexSize));

        vkUnmapMemory(device, vertexStaging.memory);

        // Upload index data into staging memory.
        mapped = nullptr;

        mapResult = vkMapMemory(device, indexStaging.memory, 0, indexSize, 0, &mapped);

        if (mapResult != VK_SUCCESS)
        {
            logger::Log::error("Failed to map Vulkan index staging memory (VkResult %d)", static_cast<int>(mapResult));
            cleanup();
            return;
        }

        std::memcpy(mapped, asset.m_indices.data(), static_cast<size_t>(indexSize));

        vkUnmapMemory(device, indexStaging.memory);

        // Submit both copies and wait for completion.
        VkCommandBuffer commandBuffer = ctx.beginSingleTimeCommands();

        ctx.copyBuffer(commandBuffer, vertexStaging.buffer, newVertex.buffer, vertexSize);
        ctx.copyBuffer(commandBuffer, indexStaging.buffer, newIndex.buffer, indexSize);

        ctx.endSingleTimeCommands(commandBuffer);

        // The GPU copies have completed.
        destroy(vertexStaging);
        destroy(indexStaging);

        // Commit the replacement only after successful upload.
        // Existing buffers are retired through Aiko's frame lifecycle.
        unload();

        m_vertexBuffer = newVertex.buffer;
        m_vertexMemory = newVertex.memory;
        m_indexBuffer = newIndex.buffer;
        m_indexMemory = newIndex.memory;
        m_indexCount = static_cast<u32>(asset.m_indices.size());

        // Ownership now belongs to this VulkanMeshImpl.
        newVertex = {};
        newIndex = {};
    }

    RenderResourceId VulkanMeshImpl::id()
    {
        return static_cast<uint>(reinterpret_cast<uintptr_t>(m_vertexBuffer));
    }

}
