#pragma once

#include <memory>
#include <vector>
#include <string>

#include "aiko_types.h"
#include "systems/base_system.h"
#include "models/game_object.h"
#include "models/mesh.h"
#include "models/light.h"
#include "modules/renderer/render_module.h"

class SceneModule;

class RenderSystem : public BaseSystem
{
public:

    friend class Mesh;
    friend class MeshComponent;

    RenderSystem();
    virtual ~RenderSystem() = default;

    virtual void init() override;
    virtual void update() override;

    aiko::AikoPtr<Mesh> createMesh();
    aiko::AikoPtr<Light> createLight();

    void add(Light*);
    void render(MeshComponent*);

protected:

    virtual void connect(ModuleConnector*, SystemConnector*) override;

private:

    const int tileSize = 10;

    void GenerateCelular();
    void RegenerateSeeds();

    aiko::AikoPtr<RenderModule> m_renderModule;
    std::vector<aiko::Vector2> m_seeds;
    RenderComponentTexture* m_rendererComponentTexture;

};