#pragma once

#include <memory>
#include <unordered_map>

#include <aiko_types.h>
#include <aiko_renderer.h>

#include "components/compute_shader_component.h"
#include "components/mesh_component.h"
#include "components/model_component.h"
#include "components/sprite_component.h"
#include "systems/base_system.h"
#include "models/game_object.h"
#include "models/model.h"
#include "utils/primitive_mesh_cache.h"

namespace aiko
{

    class SceneSystem;
    class RenderModule;
    class AssetsManagerModule;

    class RenderSystem : public BaseSystem
    {
    public:

        friend class Mesh;
        friend class Shader;

        RenderSystem();
        virtual ~RenderSystem() = default;

        virtual void init() override;
        virtual void dispose() override;

        void render(const Transform& trans, const Mesh&, const Material&);
        void render(const Transform& trans, const Model& model);
        void render(const Transform& trans, const Model& model, const MaterialInstance&);

        void render(const Transform& trans, const MeshComponent& meshComponent);
        void render(const Transform& trans, const ModelComponent& modelComponent);
        void render(const Transform& trans, const SpriteComponent& meshComponent);

        void dispatch(const ComputePass& pass, const AssetId& shaderId);
        void dispatch(const ComputePass& pass, const ComputeShaderComponent& component);

        void requestReadback(const ComputeReadbackRequest&);
        bool pollReadback(ComputeReadbackResult&);

        const FrameBuffer& getTargetTexture() const;

        Camera* getMainCamera();

        void clearCaches();

    protected:

        virtual void connect(ModuleConnector*, SystemConnector*) override;

    private:

        struct CachedMaterialEntry
        {
            AikoUPtr<Material> material;
            AssetId textureId = InvalidAssetId;
        };

        AssetsManagerModule* m_assetManagerModule;
        RenderModule* m_renderModule;
        SceneSystem* m_sceneSystem;

        std::unordered_map<string, CachedMaterialEntry> m_materialCache;

        Material& resolveCachedMaterial(const MaterialAsset& materialAsset, const MaterialInstance& materialInstance);

    // ---------------------------------------------------
    //                   PRIMITIVES
    // ---------------------------------------------------

        PrimitiveMeshCache m_primitiveMeshCache;
        TransientTopology m_defaultTransientTopology = TransientTopology::Triangles;
        Material m_materialPrimitives;
        Material& resolvePrimitiveMaterial(Material* material);

    public:

        void renderPoint(vec3 pos, Material* material = nullptr);
        void renderLine(vec3 start, vec3 end, Material* material = nullptr);
        void renderTriangle(vec3 pos, vec3 size, Material* material = nullptr);
        void renderRectangle(vec3 pos, vec3 size, Material* material = nullptr);
        void renderCircle(vec3 pos, vec3 size, uint segments, Material* material = nullptr);
        void renderNgon(vec3 pos, vec3 size, uint polygons, Material* material = nullptr);
        void renderGrid(vec3 pos, vec3 size, ivec2 resolution, Material* material = nullptr);
        void renderPyramid(vec3 pos, vec3 size, Material* material = nullptr);
        void renderCube(vec3 pos, vec3 size, Material* material = nullptr);
        void renderSphere(vec3 pos, vec3 size, int = 25, Material* material = nullptr);
        void renderPolygon(vec3 pos, vec3 size, int rings, int sectors, Material* material = nullptr);
        void renderCylinder(vec3 pos, vec3 size, uint sectors, Material* material = nullptr);
        void renderTorus(vec3 pos, vec3 size, Material* material = nullptr);
        void renderKnot(vec3 pos, vec3 size, Material* material = nullptr);


    };

}
