#pragma once

#include <vector>
#include <unordered_map>

#include "scene_render_types.h"

namespace aiko
{

    class Mesh;
    class Material;
    class TransforComponent;

    class SceneRenderWorld
    {
    public:
        void clear();

        void add(TransforComponent* trans, Mesh* mesh, Material* mat);

        const std::vector<RenderBatch>& batches() const;

    private:

        std::unordered_map<RenderBatchKey, RenderBatch, RenderBatchKeyHash> m_batchMap;

        std::vector<RenderBatch> m_batches;
    };
}
