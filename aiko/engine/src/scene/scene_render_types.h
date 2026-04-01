#pragma once

namespace aiko
{

    class Mesh;
    class Material;
    class TransforComponent;

    struct RenderInstance
    {
        TransforComponent* transform = nullptr;
    };

    struct RenderBatchKey
    {
        Mesh* mesh = nullptr;
        Material* material = nullptr;

        bool operator==(const RenderBatchKey& other) const
        {
            return mesh == other.mesh && material == other.material;
        }
    };

    struct RenderBatchKeyHash
    {
        size_t operator()(const RenderBatchKey& k) const
        {
            return std::hash<void*>()(k.mesh) ^ (std::hash<void*>()(k.material) << 1);
        }
    };

    struct RenderBatch
    {
        Mesh* mesh = nullptr;
        Material* material = nullptr;
        vector<RenderInstance> instances;

    };

}
