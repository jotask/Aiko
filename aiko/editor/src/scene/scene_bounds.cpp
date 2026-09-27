#include "scene_bounds.h"

#include "core/editor_context.h"

#include "components/mesh_component.h"
#include "components/model_component.h"
#include "components/sprite_component.h"
#include "models/game_object.h"
#include "systems/asset_system.h"

#include <assets/types/mesh_asset.h>

#include <algorithm>

namespace aiko::editor
{

    namespace
    {

        void expandBounds(Bounds& result, bool& hasBounds, const Bounds& bounds)
        {
            if (!hasBounds)
            {
                result = bounds;
                hasBounds = true;
                return;
            }

            result.min.x = std::min(result.min.x, bounds.min.x);
            result.min.y = std::min(result.min.y, bounds.min.y);
            result.min.z = std::min(result.min.z, bounds.min.z);
            result.max.x = std::max(result.max.x, bounds.max.x);
            result.max.y = std::max(result.max.y, bounds.max.y);
            result.max.z = std::max(result.max.z, bounds.max.z);
        }

        bool getMeshBounds(AssetSystem& assetSystem, const AssetId& meshId, const mat4& worldMatrix, Bounds& bounds)
        {
            if (meshId == InvalidAssetId)
            {
                return false;
            }

            if (!assetSystem.isLoaded<MeshAsset>(meshId))
            {
                return false;
            }

            const MeshAsset& mesh = assetSystem.get<MeshAsset>(meshId);

            if (mesh.m_vertices.empty())
            {
                return false;
            }

            const Bounds localBounds = math::calculateBounds(mesh.m_vertices);

            bounds = math::transformBounds(localBounds, worldMatrix);

            return true;
        }

        bool getSpriteBounds(const SpriteComponent& sprite, const mat4& worldMatrix, Bounds& bounds)
        {
            if (!sprite.isActiveAndEnabled())
            {
                return false;
            }

            if (sprite.getTextureId() == InvalidAssetId)
            {
                return false;
            }

            const vec2& size = sprite.getSize();
            const vec2& pivot = sprite.getPivot();
            const float left = -pivot.x * size.x;
            const float right = left + size.x;
            const float bottom = -pivot.y * size.y;
            const float top = bottom + size.y;

            const Bounds localBounds =
            {
                {
                    left,
                    bottom,
                    0.0f
                },
                {
                    right,
                    top,
                    0.0f
                }
            };

            bounds = math::transformBounds(localBounds, worldMatrix);

            return true;
        }

    }

    bool calculateSceneObjectBounds(EditorContext& context, const GameObject& object, Bounds& bounds)
    {
        if (!object.isActiveInHierarchy())
        {
            return false;
        }

        AssetSystem& assetSystem = context.assetSystem();

        const mat4 worldMatrix = object.transform().getWorldMatrix();

        bool hasBounds = false;

        //
        // Mesh components
        //

        for (const MeshComponent* mesh : object.getComponents<MeshComponent>())
        {
            if (mesh == nullptr || !mesh->isActiveAndEnabled())
            {
                continue;
            }

            Bounds meshBounds;

            if (!getMeshBounds(assetSystem, mesh->getMeshId(), worldMatrix, meshBounds))
            {
                continue;
            }

            expandBounds(bounds, hasBounds, meshBounds);
        }

        //
        // Model components
        //

        for (const ModelComponent* model : object.getComponents<ModelComponent>())
        {
            if (model == nullptr || !model->isActiveAndEnabled())
            {
                continue;
            }

            const AssetId& modelId = model->getModelId();

            if (modelId == InvalidAssetId || !assetSystem.isLoaded<ModelAsset>(modelId))
            {
                continue;
            }

            const ModelAsset& modelAsset = assetSystem.get<ModelAsset>(modelId);

            for (const ModelAsset::SubMesh& subMesh : modelAsset.submeshes)
            {
                Bounds meshBounds;

                if (!getMeshBounds(assetSystem, subMesh.meshId, worldMatrix, meshBounds))
                {
                    continue;
                }

                expandBounds(bounds, hasBounds, meshBounds);
            }
        }

        //
        // Sprite components
        //

        for (const SpriteComponent* sprite : object.getComponents<SpriteComponent>())
        {
            if (sprite == nullptr)
            {
                continue;
            }

            Bounds spriteBounds;

            if (!getSpriteBounds(*sprite, worldMatrix, spriteBounds))
            {
                continue;
            }

            expandBounds(bounds, hasBounds, spriteBounds);
        }

        return hasBounds;
    }

}