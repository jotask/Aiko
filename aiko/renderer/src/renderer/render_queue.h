#pragma once

#include <vector>

#include "types/aiko_renderer_types.h"
#include "types/draw_types.h"
#include "types/transient_types.h"

namespace aiko
{
    class Mesh;
    class Material;

    class RenderQueue
    {
    public:
        void clear();

        void submit(const Mesh& mesh, const Material& material, const mat4& transform);
        void submitInstanced(const Mesh& mesh, const Material& material, const void* data, u32 instanceCount, u16 stride);
        void submitTransient(TransientDrawDesc desc);

        void submitGpuInstances(const GpuInstanceDrawDesc& desc);
        void submitGpuBillboards(const GpuBillboardDrawDesc& desc);
        void submitGpuVertices(const GpuVertexDrawDesc& desc);

        PreparedScenePass buildScenePass();

    private:
        std::vector<RenderItem> m_renderItems;

        std::vector<InstanceItem> m_instancedItems;
        std::vector<u8> m_instanceData;

        std::vector<GpuInstanceDrawDesc> m_gpuInstanceDraws;
        std::vector<GpuBillboardDrawDesc> m_gpuBillboardDraws;
        std::vector<GpuVertexDrawDesc> m_gpuVertexDraws;

        std::vector<TransientDrawDesc> m_transientDraws;

        std::vector<u8> m_mergedInstanceData;
    };
}
