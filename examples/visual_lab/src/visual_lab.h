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

        void initCamera();
        void initLighting();
        void initPbrMaterials();
        void initTextureTests();

        void renderImgui();
        void renderPbrMatrix();
        void renderTextureTests();

        std::array<Material, MaterialCount> m_pbrMaterials;
        Mesh m_sphereMesh;

        std::array<Material, TextureTestCount> m_textureTestMaterials;
    };
}
