#pragma once

#include "ui_draw_list.h"
#include "models/material.h"

#include <deque>

namespace aiko
{

    namespace renderer
    {
        class IRenderDevice;
    }

    class UIRenderer
    {
    public:

        void beginFrame();

        void setShader(AssetId shaderId);

        void render(renderer::IRenderDevice& device, const UIDrawList& drawList, const ivec2& surfaceSize);

        const Material& material() const
        {
            return m_baseMaterial;
        }

    private:

        Material m_baseMaterial;
        std::deque<Material> m_frameMaterials;
    };

}
