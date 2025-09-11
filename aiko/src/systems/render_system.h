#pragma once

#include <memory>
#include <vector>

#include "aiko_types.h"
#include "systems/base_system.h"
#include "models/game_object.h"
#include "models/mesh.h"
#include "models/light.h"
#include "models/shader.h"
#include "models/texture.h"
#include "types/textures.h"
#include "modules/render/render_module.h"
#include "types/asset_type.h"

namespace aiko
{

    class CameraSystem;

    class RenderSystem : public BaseSystem
    {
    public:

        friend class Mesh;
        friend class Shader;
        friend class MeshComponent;

        RenderSystem();
        virtual ~RenderSystem() = default;

        virtual void init() override;
        virtual void update() override;
        virtual void render() override;

        Mesh createMesh(Mesh::MeshType type);
        void refreshMesh(Mesh*);
        aiko::AikoPtr<Light> createLight();
        texture::PboTexture createPboTexture(uint16_t width, uint16_t height);

        void updatePbo(texture::PboTexture text, std::vector<Color>&);

        void add(Light*);
        void render(Transform* trans, Mesh* mesh, Shader* shader);
        void render(Transform* trans, Mesh* mesh, Shader* shader, Texture*);
        void render(Transform* trans, Mesh* mesh, Shader* shader, texture::PboTexture*);
        void render(texture::RenderTexture2D&, Shader*);
        void renderToFullScreen(Shader*);

        texture::RenderTexture2D* getTargetTexture() const;

        void unloadShader(Shader* data);

        Camera* getMainCamera();

        // Font
        void renderText(string, float, float);

    protected:
    
        virtual void connect(ModuleConnector*, SystemConnector*) override;
    
    private:
    
        RenderModule* m_renderModule;
        CameraSystem* m_cameraSystem;

    // ---------------------------------------------------
    //                   PRIMITIVES
    // ---------------------------------------------------

        #define PRIMITIVE_FNT_DEFAULT_ARGS Color color = WHITE, bool border = false, float border_thickness = 0.05f
        #define PRIMITIVE_FNT(method_name, ...) void method_name( __VA_ARGS__, PRIMITIVE_FNT_DEFAULT_ARGS ) 

        Shader m_quadShaderPrimitives;
        void setPrimitiveShaderData(bool border, float thickness, Color color);
        void initPrimitives();

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
