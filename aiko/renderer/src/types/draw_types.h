#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include <aiko_types.h>
#include <math/math_vector.h>

#include "types/color.h"
#include "types/transient_types.h"

namespace aiko
{
    class ComputeBuffer;
    class Mesh;
    class Material;

    struct GpuReadBufferBinding
    {
        uint8_t slot = 0;
        const ComputeBuffer* buffer = nullptr;
    };

    struct GpuVertexDrawDesc
    {
        const Material* material = nullptr;

        const ComputeBuffer* vertexBuffer = nullptr;
        uint32_t vertexCount = 0;

        const ComputeBuffer* indexBuffer = nullptr;
        uint32_t indexCount = 0;

        const ComputeBuffer* indirectBuffer = nullptr;

        TransientTopology topology = TransientTopology::Points;
    };

    struct GpuInstanceDrawDesc
    {
        const Mesh* mesh = nullptr;
        const Material* material = nullptr;

        std::vector<GpuReadBufferBinding> readBuffers;

        uint32_t instanceCount = 0;
    };

    struct GpuBillboardDrawDesc
    {
        const Material* material = nullptr;
        const ComputeBuffer* positionBuffer = nullptr;
        uint32_t instanceCount = 0;
    };

    struct RenderItem
    {
        const Mesh* mesh = nullptr;
        const Material* material = nullptr;
        mat4 transform = mat4(1.0f);
    };

    struct MeshDrawPacket
    {
        const Mesh* mesh = nullptr;
        const Material* material = nullptr;
        mat4 world = mat4(1.0f);
    };

    struct InstancedDrawPacket
    {
        const Mesh* mesh = nullptr;
        const Material* material = nullptr;

        const void* data = nullptr;

        uint32_t instanceCount = 0;
        uint16_t stride = 0;
    };

    struct InstanceData
    {
        vec3 position;
        vec3 rotation;
        vec3 scale;
        Color color;
    };

    struct InstanceItem
    {
        const Mesh* mesh = nullptr;
        const Material* material = nullptr;

        size_t dataOffset = 0;
        size_t byteCount = 0;

        uint32_t count = 0;
        uint16_t stride = 0;
    };
}
