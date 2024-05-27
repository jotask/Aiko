#pragma once

#include "windows/editor_menu_item/menu_item.h"

namespace aiko
{
    namespace editor
    {
        class AikoEditor;
        class AssetManager : public MenuItem
        {
        public:
            AssetManager(AikoEditor*);
            ~AssetManager() = default;
            virtual void render() override;
        };

    }
}
