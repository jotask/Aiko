#pragma once

#include <string>
#include <vector>

#include <aiko_includes.h>

#include "windows/editor_menu_item/menu_item.h"

namespace aiko
{
    namespace editor
    {
        class AikoEditor;
        class AssetManager : public MenuItem
        {
        public:

            struct Asset
            {
                uuid::Uuid  uid;
                std::string name;
                std::string path;
                std::string type;
            };

            AssetManager(AikoEditor*);
            ~AssetManager() = default;
            virtual void render() override;

        private:
            std::vector<Asset> assets;
        };

    }
}
