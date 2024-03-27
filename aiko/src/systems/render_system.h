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
#include "modules/render_module.h"
#include "types/asset_type.h"

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

        void render(texture::RenderTexture2D&, Shader*);

        aiko::AikoPtr<Shader> createShader(const char* vs, const char* fs);
        void unloadShader(Shader& data);
        int getShaderLocation(Shader& shader, const char* uniformName);
        void setShaderUniformValue(Shader& shader, int locIndex, const void* value, aiko::ShaderUniformDataType uniformType);
        void setShaderUniformValueV(Shader& shader, int locIndex, const void* value, aiko::ShaderUniformDataType uniformType, int count);

    protected:
    
        virtual void connect(ModuleConnector*, SystemConnector*) override;
    
    private:
    
        RenderModule* m_renderModule;
    
    };

}
