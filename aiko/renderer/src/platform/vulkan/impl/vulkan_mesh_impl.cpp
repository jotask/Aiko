#include "vulkan_mesh_impl.h"

#include "models/mesh.h"
#include "platform/vulkan/vulkan_context.h"
#include "platform/vulkan/vulkan_types.h"

#include <assets/mesh_tangents.h>
#include <assets/types/mesh_asset.h>
#include <logger/logger.h>

#include <cstring>
#include <limits>

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

        vector<VulkanSkinVertex> buildSkinVertices(const MeshAsset& asset)
        {
            vector<VulkanSkinVertex> vertices;
            vertices.reserve(asset.m_vertices.size());
            for (size_t i = 0; i < asset.m_vertices.size(); ++i)
            {
                VulkanSkinVertex v{};
                v.jointIndices = asset.m_jointIndices[i];
                v.jointWeights = asset.m_jointWeights[i];
                vertices.push_back(v);
            }

            return vertices;
        }

        // Dense, target-major storage. Sparse/active-target optimization can
        // happen later without changing the imported MorphTargetAsset format.
        std::vector<VulkanMorphDelta> buildMorphDeltas(const MeshAsset& asset)
        {
            const size_t vertexCount = asset.m_vertices.size();
            std::vector<VulkanMorphDelta> deltas;
            deltas.reserve(vertexCount * asset.m_morphTargets.size());

            for (const MorphTargetAsset& target : asset.m_morphTargets)
            {
                AIKO_ASSERT(target.positionDeltas.size() == vertexCount, "Morph position count must match mesh vertex count");
                AIKO_ASSERT(target.normalDeltas.empty() || target.normalDeltas.size() == vertexCount, "Morph normal count must match mesh vertex count");

                for (size_t vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
                {
                    const vec3& position = target.positionDeltas[vertexIndex];
                    const vec3 normal = target.normalDeltas.empty()
                        ? vec3{0.0f, 0.0f, 0.0f}
                        : target.normalDeltas[vertexIndex];

                    deltas.push_back(VulkanMorphDelta{
                        .position = vec4{position.x, position.y, position.z, 0.0f},
                        .normal = vec4{normal.x, normal.y, normal.z, 0.0f},
                    });
                }
            }

            return deltas;
        }
    }


    VulkanMeshImpl::VulkanMeshImpl(const Mesh* mesh)
        : IMeshImpl(mesh)
    {

    }

    VulkanMeshImpl::~VulkanMeshImpl()
    {
        AIKO_ASSERT(m_vertexBuffer == VK_NULL_HANDLE && m_vertexMemory == VK_NULL_HANDLE && m_indexBuffer == VK_NULL_HANDLE && m_indexMemory == VK_NULL_HANDLE && m_skinBuffer == VK_NULL_HANDLE && m_skinMemory == VK_NULL_HANDLE && m_morphBuffer == VK_NULL_HANDLE && m_morphMemory == VK_NULL_HANDLE, "VulkanMeshImpl destroyed without unload()");
    }

    bool VulkanMeshImpl::isValid() const
    {
        const bool geometryValid =
            m_vertexBuffer != VK_NULL_HANDLE &&
            m_vertexMemory != VK_NULL_HANDLE &&
            m_indexBuffer != VK_NULL_HANDLE &&
            m_indexMemory != VK_NULL_HANDLE &&
            m_indexCount > 0;

        const bool skinValid = m_hasSkinning
            ? m_skinBuffer != VK_NULL_HANDLE &&
              m_skinMemory != VK_NULL_HANDLE
            : m_skinBuffer == VK_NULL_HANDLE &&
              m_skinMemory == VK_NULL_HANDLE;

        const bool morphValid = m_morphTargetCount > 0
            ? m_morphBuffer != VK_NULL_HANDLE &&
              m_morphMemory != VK_NULL_HANDLE &&
              m_morphVertexCount > 0
            : m_morphBuffer == VK_NULL_HANDLE &&
              m_morphMemory == VK_NULL_HANDLE &&
              m_morphVertexCount == 0;

        return geometryValid && skinValid && morphValid;
    }

    void VulkanMeshImpl::unload()
    {
        if (m_vertexBuffer == VK_NULL_HANDLE &&
            m_vertexMemory == VK_NULL_HANDLE &&
            m_indexBuffer == VK_NULL_HANDLE &&
            m_indexMemory == VK_NULL_HANDLE &&
            m_skinBuffer == VK_NULL_HANDLE &&
            m_skinMemory == VK_NULL_HANDLE &&
            m_morphBuffer == VK_NULL_HANDLE &&
            m_morphMemory == VK_NULL_HANDLE)
        {
            m_indexCount = 0;
            m_hasSkinning = false;
            m_morphTargetCount = 0;
            m_morphVertexCount = 0;
            return;
        }

        const VkBuffer vertexBuffer = m_vertexBuffer;
        const VkDeviceMemory vertexMemory = m_vertexMemory;
        const VkBuffer indexBuffer = m_indexBuffer;
        const VkDeviceMemory indexMemory = m_indexMemory;
        const VkBuffer skinBuffer = m_skinBuffer;
        const VkDeviceMemory skinMemory = m_skinMemory;
        const VkBuffer morphBuffer = m_morphBuffer;
        const VkDeviceMemory morphMemory = m_morphMemory;

        m_vertexBuffer = VK_NULL_HANDLE;
        m_vertexMemory = VK_NULL_HANDLE;
        m_indexBuffer = VK_NULL_HANDLE;
        m_indexMemory = VK_NULL_HANDLE;
        m_skinBuffer = VK_NULL_HANDLE;
        m_skinMemory = VK_NULL_HANDLE;
        m_morphBuffer = VK_NULL_HANDLE;
        m_morphMemory = VK_NULL_HANDLE;
        m_morphTargetCount = 0;
        m_morphVertexCount = 0;
        m_indexCount = 0;
        m_hasSkinning = false;

        VulkanContext& ctx = VulkanContext::current();

        ctx.retireBuffer(vertexBuffer, vertexMemory);
        ctx.retireBuffer(indexBuffer, indexMemory);
        if (skinBuffer != VK_NULL_HANDLE)
        {
            ctx.retireBuffer(skinBuffer, skinMemory);
        }
        if (morphBuffer != VK_NULL_HANDLE)
        {
            ctx.retireBuffer(morphBuffer, morphMemory);
        }
    }

    void VulkanMeshImpl::refresh(const MeshAsset& asset)
    {
        AIKO_ASSERT(!asset.m_vertices.empty(), "Mesh has no vertices");
        AIKO_ASSERT(!asset.m_indices.empty(), "Mesh has no indices");
        AIKO_ASSERT(asset.m_tangents.empty() || asset.m_tangents.size() == asset.m_vertices.size(), "Mesh tangent count must match vertex count");

        bool hasSkinning = asset.m_jointIndices.empty() == false;

        AIKO_ASSERT(asset.m_jointIndices.empty() == asset.m_jointWeights.empty(), "Skinning indices and weights must both be present or absent");
        if (hasSkinning == true)
        {
            AIKO_ASSERT(asset.m_jointIndices.size() == asset.m_vertices.size(), "Skinning joint count must match vertex count");
            AIKO_ASSERT(asset.m_jointWeights.size() == asset.m_vertices.size(), "Skinning weight count must match vertex count");
        }

        for (const uint32_t index : asset.m_indices)
        {
            AIKO_ASSERT(index < asset.m_vertices.size(), "Mesh index out of vertex range");
        }

        const bool hasMorphTargets = !asset.m_morphTargets.empty();
        if (hasMorphTargets)
        {
            AIKO_ASSERT(asset.m_vertices.size() <= std::numeric_limits<uint32_t>::max(), "Morph mesh has too many vertices");
            AIKO_ASSERT(asset.m_morphTargets.size() <= std::numeric_limits<uint32_t>::max(), "Morph mesh has too many targets");
            AIKO_ASSERT(asset.m_morphTargets.size() <= std::numeric_limits<size_t>::max() / asset.m_vertices.size(), "Morph entry count overflow");

            for (const MorphTargetAsset& target : asset.m_morphTargets)
            {
                AIKO_ASSERT(target.positionDeltas.size() == asset.m_vertices.size(), "Invalid morph position count");
                AIKO_ASSERT(target.normalDeltas.empty() || target.normalDeltas.size() == asset.m_vertices.size(), "Invalid morph normal count");
            }
        }

        VulkanContext& ctx = VulkanContext::current();
        const VkDevice device = ctx.device();

        const vector<VulkanVertex> vertices = buildVertices(asset);
        const std::vector<VulkanSkinVertex> skinnedVertices = hasSkinning ? buildSkinVertices(asset) : std::vector<VulkanSkinVertex>{};
        const std::vector<VulkanMorphDelta> morphDeltas = hasMorphTargets ? buildMorphDeltas(asset) : std::vector<VulkanMorphDelta>{};

        const VkDeviceSize vertexSize = sizeof(VulkanVertex) * vertices.size();
        const VkDeviceSize indexSize = sizeof(uint32_t) * asset.m_indices.size();
        const VkDeviceSize skinSize = sizeof(VulkanSkinVertex) * skinnedVertices.size();
        const VkDeviceSize morphSize = sizeof(VulkanMorphDelta) * morphDeltas.size();

        if (hasMorphTargets)
        {
            VkPhysicalDeviceProperties properties{};
            vkGetPhysicalDeviceProperties(ctx.physicalDevice(), &properties);
            AIKO_ASSERT(morphSize <= properties.limits.maxStorageBufferRange, "Morph buffer exceeds Vulkan maxStorageBufferRange");
        }

        struct BufferAllocation
        {
            VkBuffer buffer = VK_NULL_HANDLE;
            VkDeviceMemory memory = VK_NULL_HANDLE;
        };

        BufferAllocation vertexStaging;
        BufferAllocation indexStaging;
        BufferAllocation newVertex;
        BufferAllocation newIndex;
        BufferAllocation skinStaging;
        BufferAllocation newSkin;
        BufferAllocation morphStaging;
        BufferAllocation newMorph;

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
            destroy(skinStaging);
            destroy(morphStaging);
            destroy(newVertex);
            destroy(newIndex);
            destroy(newSkin);
            destroy(newMorph);
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

        if (hasSkinning == true)
        {

            if (!allocate(skinStaging, skinSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingProperties, "skin staging") ||
                !allocate(newSkin, skinSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, deviceProperties, "new skin"))
            {
                cleanup();
                return;
            }
        }

        if (hasMorphTargets)
        {
            if (!allocate(morphStaging, morphSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingProperties, "morph staging") ||
                !allocate(newMorph, morphSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, deviceProperties, "morph"))
            {
                cleanup();
                return;
            }
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

        if (hasSkinning)
        {
            mapped = nullptr;
            mapResult = vkMapMemory(device, skinStaging.memory, 0, skinSize, 0, &mapped);

            if (mapResult != VK_SUCCESS)
            {
                logger::Log::error("Failed to map Vulkan skin staging memory (VkResult %d)", static_cast<int>(mapResult));
                cleanup();
                return;
            }

            std::memcpy(mapped, skinnedVertices.data(), static_cast<size_t>(skinSize));

            vkUnmapMemory(device, skinStaging.memory);
        }

        if (hasMorphTargets)
        {
            mapped = nullptr;
            mapResult = vkMapMemory(device, morphStaging.memory, 0, morphSize, 0, &mapped);

            if (mapResult != VK_SUCCESS)
            {
                logger::Log::error("Failed to map Vulkan morph staging memory (VkResult %d)", static_cast<int>(mapResult));
                cleanup();
                return;
            }

            std::memcpy(mapped, morphDeltas.data(), static_cast<size_t>(morphSize));
            vkUnmapMemory(device, morphStaging.memory);
        }

        // Submit all copies and wait for completion.
        VkCommandBuffer commandBuffer = ctx.beginSingleTimeCommands();

        ctx.copyBuffer(commandBuffer, vertexStaging.buffer, newVertex.buffer, vertexSize);
        ctx.copyBuffer(commandBuffer, indexStaging.buffer, newIndex.buffer, indexSize);

        if (hasSkinning)
        {
            ctx.copyBuffer(commandBuffer, skinStaging.buffer, newSkin.buffer, skinSize);
        }
        if (hasMorphTargets)
        {
            ctx.copyBuffer(commandBuffer, morphStaging.buffer, newMorph.buffer, morphSize);
        }

        ctx.endSingleTimeCommands(commandBuffer);

        // The GPU copies have completed.
        destroy(vertexStaging);
        destroy(indexStaging);
        destroy(skinStaging);
        destroy(morphStaging);

        // Commit the replacement only after successful upload.
        // Existing buffers are retired through Aiko's frame lifecycle.
        unload();

        m_vertexBuffer = newVertex.buffer;
        m_vertexMemory = newVertex.memory;
        m_indexBuffer = newIndex.buffer;
        m_indexMemory = newIndex.memory;
        m_indexCount = static_cast<u32>(asset.m_indices.size());
        m_skinBuffer = newSkin.buffer;
        m_skinMemory = newSkin.memory;
        m_hasSkinning = hasSkinning;
        m_morphBuffer = newMorph.buffer;
        m_morphMemory = newMorph.memory;
        m_morphTargetCount = static_cast<uint32_t>(asset.m_morphTargets.size());
        m_morphVertexCount = hasMorphTargets ? static_cast<uint32_t>(asset.m_vertices.size()) : 0;

        if (hasMorphTargets)
        {
            logger::Log::infoFmt("Uploaded {} morph targets to GPU ({} vertices each, {} bytes)",
                m_morphTargetCount, m_morphVertexCount, static_cast<size_t>(morphSize));
        }

        // Ownership now belongs to this VulkanMeshImpl.
        newVertex = {};
        newIndex = {};
        newSkin = {};
        newMorph = {};
    }

    RenderResourceId VulkanMeshImpl::id()
    {
        return static_cast<uint>(reinterpret_cast<uintptr_t>(m_vertexBuffer));
    }

}
