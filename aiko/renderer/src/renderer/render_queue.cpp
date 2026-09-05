#include "render_queue.h"

#include <algorithm>
#include <intrumentor/profiler.h>

#include "models/material.h"
#include "models/mesh.h"

namespace aiko
{
    void RenderQueue::submit(const Mesh& mesh, const Material& material, const mat4& transform)
    {
        m_renderItems.push_back(RenderItem
        {
            .mesh = &mesh,
            .material = &material,
            .transform = transform
        });
    }

    void RenderQueue::submitInstanced(const Mesh& mesh, const Material& material, const void* data, u32 instanceCount, u16 stride)
    {
        if (data == nullptr || instanceCount == 0 || stride == 0)
        {
            return;
        }

        const size_t byteCount = size_t(instanceCount) * size_t(stride);

        if (byteCount == 0)
        {
            return;
        }

        const size_t offset = m_instanceData.size();

        m_instanceData.resize(offset + byteCount);

        memcpy(m_instanceData.data() + offset, data, byteCount);

        m_instancedItems.push_back(InstanceItem
        {
            .mesh = &mesh,
            .material = &material,
            .dataOffset = offset,
            .byteCount = byteCount,
            .count = instanceCount,
            .stride = stride
        });
    }

    void RenderQueue::submitTransient(TransientDrawDesc desc)
    {
        m_transientDraws.push_back(std::move(desc));
    }

    void RenderQueue::submitGpuInstances(const GpuInstanceDrawDesc& desc)
    {
        m_gpuInstanceDraws.push_back(desc);
    }

    void RenderQueue::submitGpuBillboards(const GpuBillboardDrawDesc& desc)
    {
        m_gpuBillboardDraws.push_back(desc);
    }

    void RenderQueue::submitGpuVertices(const GpuVertexDrawDesc& desc)
    {
        m_gpuVertexDraws.push_back(desc);
    }

    void RenderQueue::clear()
    {
        m_renderItems.clear();

        m_instancedItems.clear();
        m_instanceData.clear();
        m_mergedInstanceData.clear();

        m_gpuInstanceDraws.clear();
        m_gpuBillboardDraws.clear();
        m_gpuVertexDraws.clear();

        m_transientDraws.clear();
    }

    PreparedScenePass RenderQueue::buildScenePass()
    {
        AIKO_FUNCTION_PROFILE
        PreparedScenePass passData;

        passData.gpuInstances.reserve(m_gpuInstanceDraws.size());
        for (const GpuInstanceDrawDesc& desc : m_gpuInstanceDraws)
        {
            passData.gpuInstances.push_back(&desc);
        }

        passData.gpuBillboards.reserve(m_gpuBillboardDraws.size());
        for (const GpuBillboardDrawDesc& desc : m_gpuBillboardDraws)
        {
            passData.gpuBillboards.push_back(&desc);
        }

        passData.gpuVertices.reserve(m_gpuVertexDraws.size());
        for (const GpuVertexDrawDesc& desc : m_gpuVertexDraws)
        {
            passData.gpuVertices.push_back(&desc);
        }

        passData.opaque.reserve(m_renderItems.size());
        for (const RenderItem& item : m_renderItems)
        {
            if (item.mesh == nullptr || item.material == nullptr)
            {
                continue;
            }

            passData.opaque.push_back(PreparedRenderPacket
            {
                .draw =
                {
                    .mesh = item.mesh,
                    .material = item.material,
                    .world = item.transform
                },
                .materialId = item.material->id()
            });
        }

        std::ranges::sort(passData.opaque, [](const PreparedRenderPacket& a, const PreparedRenderPacket& b)
        {
            if (a.materialId != b.materialId)
            {
                return a.materialId < b.materialId;
            }

            if (a.draw.material != b.draw.material)
            {
                return a.draw.material < b.draw.material;
            }

            return a.draw.mesh < b.draw.mesh;
        });

        {
            std::vector<PreparedInstancedPacket> sortedInstanced;
            sortedInstanced.reserve(m_instancedItems.size());

            size_t totalInstancedBytes = 0;

            for (const InstanceItem& item : m_instancedItems)
            {
                if (item.mesh == nullptr || item.material == nullptr || item.byteCount == 0)
                {
                    continue;
                }

                AIKO_ASSERT(item.dataOffset + item.byteCount <= m_instanceData.size(), "Instanced data range out of bounds");
                AIKO_ASSERT(item.byteCount == size_t(item.count) * size_t(item.stride), "Instanced byteCount mismatch");

                totalInstancedBytes += item.byteCount;

                sortedInstanced.push_back(PreparedInstancedPacket
                {
                    .draw =
                    {
                        .mesh = item.mesh,
                        .material = item.material,
                        .data = m_instanceData.data() + item.dataOffset,
                        .instanceCount = item.count,
                        .stride = item.stride
                    },
                    .materialId = item.material->id(),
                    .meshId = item.mesh->id(),
                    .mergedDataOffset = 0,
                    .mergedByteCount = 0
                });
            }

            std::ranges::sort(sortedInstanced, [](const PreparedInstancedPacket& a, const PreparedInstancedPacket& b)
            {
                if (a.draw.material != b.draw.material)
                    return a.draw.material < b.draw.material;

                if (a.draw.mesh != b.draw.mesh)
                    return a.draw.mesh < b.draw.mesh;

                return a.draw.stride < b.draw.stride;
            });

            m_mergedInstanceData.clear();
            m_mergedInstanceData.reserve(totalInstancedBytes);

            std::vector<PreparedInstancedPacket> mergedInstanced;
            mergedInstanced.reserve(sortedInstanced.size());

            for (const PreparedInstancedPacket& packet : sortedInstanced)
            {
                const size_t packetByteCount =
                    size_t(packet.draw.instanceCount) * size_t(packet.draw.stride);

                AIKO_ASSERT(packetByteCount > 0, "Merged instanced packet byteCount is zero");

                const auto* srcBytes = static_cast<const uint8_t*>(packet.draw.data);
                AIKO_ASSERT(srcBytes != nullptr, "Prepared instanced packet has null source data");

                if (mergedInstanced.empty())
                {
                    const size_t offset = m_mergedInstanceData.size();
                    m_mergedInstanceData.resize(offset + packetByteCount);
                    memcpy(m_mergedInstanceData.data() + offset, srcBytes, packetByteCount);

                    PreparedInstancedPacket mergedPacket = packet;
                    mergedPacket.mergedDataOffset = offset;
                    mergedPacket.mergedByteCount = packetByteCount;
                    mergedPacket.draw.data = m_mergedInstanceData.data() + offset;

                    mergedInstanced.push_back(std::move(mergedPacket));
                    continue;
                }

                PreparedInstancedPacket& last = mergedInstanced.back();

                const bool compatible =
                    last.draw.mesh == packet.draw.mesh &&
                    last.draw.material == packet.draw.material &&
                    last.draw.stride == packet.draw.stride;

                if (!compatible)
                {
                    const size_t offset = m_mergedInstanceData.size();
                    m_mergedInstanceData.resize(offset + packetByteCount);
                    memcpy(m_mergedInstanceData.data() + offset, srcBytes, packetByteCount);

                    PreparedInstancedPacket mergedPacket = packet;
                    mergedPacket.mergedDataOffset = offset;
                    mergedPacket.mergedByteCount = packetByteCount;
                    mergedPacket.draw.data = m_mergedInstanceData.data() + offset;

                    mergedInstanced.push_back(std::move(mergedPacket));
                    continue;
                }

                const size_t appendOffset = m_mergedInstanceData.size();
                m_mergedInstanceData.resize(appendOffset + packetByteCount);
                memcpy(m_mergedInstanceData.data() + appendOffset, srcBytes, packetByteCount);

                last.draw.instanceCount += packet.draw.instanceCount;
                last.mergedByteCount += packetByteCount;
                last.draw.data = m_mergedInstanceData.data() + last.mergedDataOffset;
            }

            passData.instanced = std::move(mergedInstanced);

        }

        {

            passData.transient.reserve(m_transientDraws.size());

            for (const TransientDrawDesc& item : m_transientDraws)
            {
                const bool hasCachedGeometry =
                    item.geometry != nullptr &&
                    !item.geometry->vertices.empty();

                const bool hasInlineGeometry =
                    !item.vertices.empty();

                if (item.material == nullptr || (!hasCachedGeometry && !hasInlineGeometry))
                {
                    continue;
                }

                passData.transient.push_back(PreparedTransientPacket
                {
                    .item = &item,
                    .materialId = item.material->id()
                });
            }

            std::ranges::sort(passData.transient, [](const PreparedTransientPacket& a, const PreparedTransientPacket& b)
            {
                if (a.materialId != b.materialId)
                {
                    return a.materialId < b.materialId;
                }

                AIKO_ASSERT(a.item != nullptr, "Prepared transient packet a has null item");
                AIKO_ASSERT(b.item != nullptr, "Prepared transient packet b has null item");

                return a.item->material < b.item->material;
            });

        }

        return passData;
    }

}
