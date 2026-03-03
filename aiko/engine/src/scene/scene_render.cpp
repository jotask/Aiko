#include "scene_render.h"

#include "aiko_renderer.h"
#include "scene.h"
#include "components/compute_shader_component.h"
#include "components/mesh_component.h"
#include "components/model_component.h"
#include "models/game_object.h"

namespace aiko
{
    void SceneRenderer::render(Scene& scene)
    {
        m_worldRenderer.clear();
        for (auto* go : scene.getObjects())
        {
            submitGameObject(go);
        }
        submitBatches();
    }

    void SceneRenderer::submitGameObject(GameObject* obj)
    {
        if (obj == nullptr)
        {
            return;
        }

        if (obj->hasComponent<TransforComponent>() == false)
        {
            logger::Log::warning("Found Game Object with no transform");
            return;
        }

        const AikoPtr<TransforComponent> transform = obj->getComponent<TransforComponent>();

        // Mesh component
        if (obj->hasComponent<MeshComponent>() == true)
        {
            AikoPtr<MeshComponent> cmp = obj->getComponent<MeshComponent>();
            m_worldRenderer.add(transform.get(), &cmp->m_mesh, &cmp->m_material);
        }

        // Model component
        if (obj->hasComponent<ModelComponent>() == true)
        {
            AikoPtr<ModelComponent> cmp = obj->getComponent<ModelComponent>();
            for (Model::MeshMatData& sub : cmp->m_model.m_meshes)
            {
                m_worldRenderer.add(transform.get(), &sub.mesh, &sub.material);
            }
        }

        // TEMPORAL
        if (obj->hasComponent<ComputeShaderComponent>() == true)
        {
            AikoPtr<ComputeShaderComponent> cmp = obj->getComponent<ComputeShaderComponent>();
            cmp->render();
        }

    }

    void SceneRenderer::submitBatches()
    {
        for (auto& batch : m_worldRenderer.batches())
        {
            const size_t count = batch.instances.size();

            // One single draw, not batching
            if (count == 1)
            {
                AikoRenderer::it().submit(
                    batch.instances.front().transform->transform,
                    *batch.mesh,
                    *batch.material
                );
                continue;
            }

            // Instancing drawing
            std::vector<InstanceData> instanceData;
            instanceData.resize(count);

            for (size_t i = 0 ; i < count; ++i )
            {
                TransforComponent* tr = batch.instances.at(i).transform;
                const InstanceData data = {
                    .position = tr->transform.position,
                    .rotation = tr->transform.rotation,
                    .scale = tr->transform.scale,
                    .color = WHITE,
                };
                instanceData.push_back(data);
            };
            AikoRenderer::it().submit(
                *batch.mesh,
                *batch.material,
                instanceData.data(),
                static_cast<uint32_t>(count),
                sizeof(InstanceData)
            );

        }
    }
}
