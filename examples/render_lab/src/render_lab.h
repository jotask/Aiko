#pragma once

#include "aiko_includes.h"
#include "layers/layer.h"
#include "models/material.h"
#include "models/mesh.h"
#include "models/texture.h"

namespace aiko
{
    class ComputeShaderComponent;
    class GameObject;
    class LightComponent;
    class RenderSystem;
    class SystemConnector;
}

namespace aiko::lab
{
    class RenderLab final : public Layer
    {
    public:
        RenderLab() = default;
        ~RenderLab() override = default;

    protected:
        void init() override;
        void update() override;
        void render() override;
        void connect(SystemConnector& systemConnector) override;

    private:
        struct LightInstance
        {
            GameObject* object = nullptr;
            LightComponent* component = nullptr;
            float angle = 0.0f;
        };

    private:
        // --------------------------------------------------
        // Setup
        // --------------------------------------------------

        void initCamera();
        void initModels();
        void initComponents();
        void initMaterials();
        void initRuntimeTextures();
        void initInstancing();
        void initLights();
        void initParticles();
        void initCompute();

        // --------------------------------------------------
        // Update
        // --------------------------------------------------

        void updateComponents();
        void updateLights();
        void updateCompute();
        void updateRuntimeTexture();

        // --------------------------------------------------
        // Render
        // --------------------------------------------------

        void renderPrimitives();
        void renderMaterialTests();
        void renderRenderStateTests();
        void renderRuntimeTextureTests();
        void renderInstancing();
        void renderLights();

    private:
        RenderSystem* m_renderSystem = nullptr;

        // Animated component tests
        GameObject* m_cubeA = nullptr;
        GameObject* m_cubeB = nullptr;
        GameObject* m_dynamicTextureObject = nullptr;

        // Lights
        std::vector<LightInstance> m_lights;

        // Compute
        ComputeShaderComponent* m_computeReadback = nullptr;
        bool m_computeReadbackPrinted = false;

        // CPU instancing
        Mesh m_instancingMesh;
        Material m_instancingMaterial;

        // Runtime texture path
        Texture m_runtimeTextureA;
        Texture m_runtimeTextureB;

        Mesh m_runtimeTextureMesh;

        Material m_runtimeTextureMaterial;
        Material m_multiTextureMaterial;

        // Material/uniform validation
        Material m_defaultMaterial;
        Material m_uniformMaterial;

        // Primitive color validation
        Material m_primitiveRedMaterial;
        Material m_primitiveGreenMaterial;
        Material m_primitiveBlueMaterial;
        Material m_primitiveYellowMaterial;

        // Lighting validation
        Material m_litMaterial;
        Material m_unlitMaterial;

        // Render state validation
        Material m_solidMaterial;
        Material m_wireframeMaterial;
        Material m_pointMaterial;

        Material m_cullNoneMaterial;
        Material m_cullFrontMaterial;
        Material m_cullBackMaterial;

        Material m_depthMaterial;
        Material m_noDepthMaterial;

        Material m_blendMaterial;
    };
}
