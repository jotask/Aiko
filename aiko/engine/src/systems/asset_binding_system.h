#pragma once

#include "systems/base_system.h"

namespace aiko
{
    class AssetSystem;
    class SceneSystem;
    class MeshComponent;
    class ModelComponent;
    class ComputeShaderComponent;
    class SpriteComponent;

    class AssetBindingSystem : public BaseSystem
    {
    public:
        AssetBindingSystem() = default;
        virtual ~AssetBindingSystem() override = default;

    protected:
        virtual void connect(ModuleConnector*, SystemConnector*) override;
        virtual void update() override;

    private:
        void resolveMesh(MeshComponent& component);
        void resolveModel(ModelComponent& component);
        void resolveSprite(SpriteComponent& component);
        void resolveComputeShader(ComputeShaderComponent& component);

        AssetSystem* m_assetSystem = nullptr;
        SceneSystem* m_sceneSystem = nullptr;
    };
}
