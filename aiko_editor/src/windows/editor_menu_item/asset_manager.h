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
                enum class AssetType { Default, Text, Texture, };
                uuid::Uuid  uid;
                std::string name;
                std::string path;
                AssetType type;
            };

            AssetManager(AikoEditor*);
            ~AssetManager() = default;
            virtual void render() override;

        private:

            void load_file(const char* file);
            void save_file();

            std::vector<Asset> assets;
            std::string file_path;
        };

    }
}
