#pragma once

#include <memory>
#include <unordered_map>

#include <aiko_types.h>
#include <models/mesh.h>
#include <models/texture.h>
#include <aiko_renderer.h>

#include "components/compute_shader_component.h"
#include "components/mesh_component.h"
#include "components/model_component.h"
#include "components/sprite_component.h"
#include "systems/base_system.h"
#include "models/game_object.h"
#include "models/model.h"

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
        virtual void update() override;
        virtual void render() override;
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

        #define PRIMITIVE_FNT_DEFAULT_ARGS Color color = AIKO_DEFAULT_PRIMITIVE_COLOR, bool border = false, float border_thickness = 0.05f
        #define PRIMITIVE_FNT(method_name, ...) void method_name( __VA_ARGS__, PRIMITIVE_FNT_DEFAULT_ARGS ) 

        Material m_materialPrimitives;

    public:

        // 2d
        PRIMITIVE_FNT(drawPoint, vec3 pos);
        PRIMITIVE_FNT(renderLine, vec3 start, vec3 end);

        PRIMITIVE_FNT(drawTriangle, vec3 pos, vec3 size);
        PRIMITIVE_FNT(drawRectangle, vec3 pos, vec3 size);
        PRIMITIVE_FNT(renderCircle, vec3 pos, vec3 size);
        PRIMITIVE_FNT(renderNgon, vec3 pos, vec3 size, uint polygons);

        // 3d
        PRIMITIVE_FNT(drawPlane, vec3 pos, vec3 size);
        PRIMITIVE_FNT(drawPyramid, vec3 pos, vec3 size);
        PRIMITIVE_FNT(drawCube, vec3 pos, vec3 size);
        PRIMITIVE_FNT(renderSphere, vec3 pos, vec3 size, int = 25);
        PRIMITIVE_FNT(renderPolygon, vec3 pos, vec3 size, int rings, int sectors);
        PRIMITIVE_FNT(renderCylinder, vec3 pos, vec3 size, uint sectors);

        PRIMITIVE_FNT(renderTorus, vec3 pos, vec3 size);
        PRIMITIVE_FNT(renderKnot, vec3 pos, vec3 size);

    private:

    };

}
