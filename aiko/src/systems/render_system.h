#pragma once

#include <memory>
#include <vector>

#include "aiko_types.h"
#include "systems/base_system.h"
#include "models/game_object.h"
#include "models/mesh.h"
#include "models/light.h"
#include "models/shader.h"
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

        aiko::AikoPtr<Mesh> createMesh(Mesh::MeshType type);
        aiko::AikoPtr<Light> createLight();
        texture::Texture createTexture();
        texture::PboTexture createPboTexture(uint16_t width, uint16_t height);

        void updatePbo(texture::PboTexture text, std::vector<Color>&);

        void add(Light*);
        void render(Transform* trans, Mesh* mesh, Shader* shader);
        void render(Transform* trans, Mesh* mesh, Shader* shader, texture::Texture*);

        texture::RenderTexture2D* getTargetTexture() const;

        void renderToFullScreen(Shader*);

        void render(texture::RenderTexture2D&, Shader*);

        AikoPtr<Shader> createShader();
        AikoPtr<Shader> createShader(const char* name);
        AikoPtr<Shader> createShader(const char* vs, const char* fs);
        void unloadShader(Shader& data);

        Camera* getMainCamera();

        // Font
        void renderText(string, float, float);

        template<class Ctx>
        Ctx* getRenderer(ContextType);

    protected:
    
        virtual void connect(ModuleConnector*, SystemConnector*) override;
    
    private:
    
        RenderModule* m_renderModule;
        CameraSystem* m_cameraSystem;
    
    };

    template<class Ctx>
    inline Ctx* RenderSystem::getRenderer(ContextType ctx)
    {
        return m_renderModule->getRenderer<Ctx>(ctx);
    }

}
