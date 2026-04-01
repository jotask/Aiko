#include "scene_render_world.h"

#include "components/transform_component.h"

namespace aiko
{
    void SceneRenderWorld::clear()
    {
        m_batchMap.clear();
        m_batches.clear();
    }

    void SceneRenderWorld::add(TransforComponent* trans, Mesh* mesh, Material* mat)
    {
        const RenderBatchKey key =
        {
            .mesh = mesh,
            .material = mat
        };
        auto& batch = m_batchMap[key];
        if (batch.mesh == nullptr)
        {
            batch.mesh = mesh;
            batch.material = mat;
        }
        batch.instances.push_back({ .transform = trans });
    }

    const vector<RenderBatch>& SceneRenderWorld::batches() const
    {
        if (m_batches.empty() == true)
        {
            for (auto& kv : m_batchMap)
            {
                const_cast<vector<RenderBatch>&>(m_batches).push_back(kv.second);
            }
        }
        return m_batches;
    }

}
