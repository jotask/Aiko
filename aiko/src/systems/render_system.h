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

    public:

        #define DEFAULT_2D_COLOR { 1.0f   , 0.686f , 0.8f    , 1.0f }
        #define DEFAULT_3D_COLOR { 1.0f   , 1.0f   , 1.0f    , 1.0f }

        Shader m_quadShader;
        Mesh m_quadMesh;

        void initPrimitives();

        // 2D
        void drawPoint(vec3, Color = DEFAULT_2D_COLOR);
        void drawTriangle(vec3, float, Color = DEFAULT_2D_COLOR);
        void drawTriangle(vec3, vec3, vec3, Color = DEFAULT_2D_COLOR);
        void drawRectangle(vec3, vec3, Color = DEFAULT_2D_COLOR, bool border = false, float border_thickness = 0.05f);
        void renderLine(vec3, vec3, Color = DEFAULT_2D_COLOR);
        void renderCircle(vec3, float, int = 50, Color = DEFAULT_2D_COLOR);
        void renderNgon(vec3, float, unsigned int, Color = DEFAULT_2D_COLOR);

        // 3d
        void drawPyramid(vec3, float baseWidth, float height, Color = DEFAULT_3D_COLOR);
        void drawCube(vec3, vec3, Color = DEFAULT_3D_COLOR);
        void renderSphere(vec3, float, int = 25, Color = DEFAULT_3D_COLOR);
        void renderPolygon(vec3, float, int rings, int sectors, Color = DEFAULT_3D_COLOR);
        void renderCylinder(vec3 pos, float radius, float height, int sectors, Color = DEFAULT_3D_COLOR);

    private:

    };

}
