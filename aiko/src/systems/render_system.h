#pragma once

#include <memory>
#include <vector>
#include <string>

#include "aiko_types.h"
#include "systems/base_system.h"
#include "models/game_object.h"
#include "models/mesh.h"
#include "models/light.h"

class SceneModule;

class RenderSystem : public BaseSystem
{
public:

    friend class Mesh;
    friend class MeshComponent;

    RenderSystem() = default;
    virtual ~RenderSystem() = default;

    aiko::AikoPtr<Mesh> createMesh();
    aiko::AikoPtr<Light> createLight();

    void add(Light*);
    void render(MeshComponent*);

protected:

    virtual void connect(ModuleConnector*, SystemConnector*) override;

private:

    aiko::AikoPtr<SceneModule> m_sceneModule;

};