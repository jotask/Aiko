#pragma once

#include <memory>
#include <vector>
#include <string>

#include "aiko_types.h"
#include "systems/base_system.h"
#include "models/game_object.h"
#include "models/mesh.h"
#include "models/light.h"
#include "models/shader.h"
#include "types/textures.h"
#include "modules/renderer/render_module.h"

namespace aiko
{

    class SceneModule;

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

        aiko::AikoPtr<Mesh> createMesh();
        aiko::AikoPtr<Light> createLight();
    
        void add(Light*);
        void render(MeshComponent*);
    
        texture::RenderTexture2D* getTargetTexture() const;

        void render(texture::RenderTexture2D&, shader::Shader&);

    protected:
    
        virtual void connect(ModuleConnector*, SystemConnector*) override;
    
    private:
    
        RenderModule* m_renderModule;
    
    };

}
