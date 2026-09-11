#pragma once

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

        const vector<RenderBatch>& batches() const;

    private:

        std::unordered_map<RenderBatchKey, RenderBatch, RenderBatchKeyHash> m_batchMap;

        vector<RenderBatch> m_batches;
    };
}
