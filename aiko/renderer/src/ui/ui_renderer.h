#pragma once

#include "ui_draw_list.h"
#include "models/material.h"

namespace aiko
{

    namespace renderer
    {
        class IRenderDevice;
    }

    class UIRenderer
    {
    public:

        void setShader(AssetId shaderId);

        void render(renderer::IRenderDevice& device, const UIDrawList& drawList);

        const Material& material() const
        {
            return m_material;
        }

    private:

        Material m_material;
    };

}
