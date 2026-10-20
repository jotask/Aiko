#pragma once

#include "aiko_includes.h"
#include "layers/layer.h"

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

        void initCamera();
        void initLighting();
        void initPbrMaterials();
        void initTextureTests();
        void initFullPbrTest();
        void initColorSpaceTest();
        void initMaterialColorTest();

        void renderImgui();
        void renderPbrMatrix();
        void renderTextureTests();
        void renderFullPbrTest();
        void renderColorSpaceTest();
        void renderMaterialColorTest();

        std::array<Material, MaterialCount> m_pbrMaterials;
        Mesh m_sphereMesh;

        std::array<Material, TextureTestCount> m_textureTestMaterials;

        std::array<Material, FullPbrTestCount> m_fullPbrMaterials;
        Mesh m_fireHydrantMesh;

        Material m_colorSpaceMaterial;
        Mesh m_colorSpaceQuad;

        Material m_materialColorMaterial;
    };
}
