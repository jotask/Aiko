#pragma once

#include "aiko_includes.h"
#include "layers/layer.h"

namespace aiko
{
    class GameObject;
    class LightComponent;
}

namespace aiko::lab
{
    class VisualLab final : public Layer
    {
    public:
        VisualLab() = default;
        ~VisualLab() override = default;

    protected:
        void init() override;
        void update() override;
        void render() override;
        void dispose() override;

    private:
        static constexpr int GridSize = 5;
        static constexpr int MaterialCount = GridSize * GridSize;
        static constexpr int TextureTestCount = 9;
        static constexpr int FullPbrTestCount = 3;

        struct ValidationLight
        {
            const char* name = "";
            LightType type = LightType::Directional;

            GameObject* object = nullptr;
            LightComponent* component = nullptr;

            bool enabled = false;

            vec3 position = vec3(0.0f);
            vec3 direction = vec3(0.0f, -1.0f, 0.0f);

            Color color = WHITE;
            float intensity = 1.0f;
            float range = 10.0f;

            float innerCos = 0.8f;
            float outerCos = 0.6f;

            bool castsShadows = true;
        };

        void initCamera();
        void initLighting();
        void initPbrMaterials();
        void initTextureTests();
        void initFullPbrTest();
        void initColorSpaceTest();
        void initMaterialColorTest();
        void initShadowValidation();
        void initShadowValidationLights();

        void syncVisualLabLighting();
        void applyValidationLight(ValidationLight& light);

        void renderImgui();
        void renderPbrMatrix();
        void renderTextureTests();
        void renderFullPbrTest();
        void renderColorSpaceTest();
        void renderMaterialColorTest();
        void renderShadowValidation();
        void renderValidationLightControls(ValidationLight& light);

        std::array<Material, MaterialCount> m_pbrMaterials;
        Mesh m_sphereMesh;

        std::array<Material, TextureTestCount> m_textureTestMaterials;

        std::array<Material, FullPbrTestCount> m_fullPbrMaterials;
        Mesh m_fireHydrantMesh;

        Material m_colorSpaceMaterial;
        Mesh m_colorSpaceQuad;

        Material m_materialColorMaterial;

        LightComponent* m_referenceLight = nullptr;

        ValidationLight m_shadowDirLight;
        ValidationLight m_shadowSpotA;
        ValidationLight m_shadowSpotB;
        ValidationLight m_shadowPointA;

        Mesh m_shadowCubeMesh;
        Mesh m_shadowGroundMesh;

        Material m_shadowCasterMaterial;
        Material m_shadowGroundMaterial;

        bool m_shadowAnimate = true;
        bool m_shadowShowGround = true;
        bool m_shadowShowSphere = true;

        float m_shadowRotation = 0.0f;
        float m_shadowHeight = 1.5f;
    };
}
