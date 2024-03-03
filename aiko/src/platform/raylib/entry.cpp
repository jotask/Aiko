#ifdef AIKO_BUILD_RAYLIB
#include "platform/platform_builder.hpp"

// Modules
#include "modules/display_module.h"
#include "modules/scene_module.h"
#include "modules/camera_module.h"
#include "modules/input_module.h"
#include "modules/asset_module.h"
#include "modules/render_module.h"

namespace aiko::modules::builder
{

    void initModules(aiko::Aiko::Modules& modules)
    {
        modules.emplace_back(std::make_unique<DisplayModule>());
        modules.emplace_back(std::make_unique<SceneModule>());
        modules.emplace_back(std::make_unique<RenderModule>());
        modules.emplace_back(std::make_unique<AssetModule>());
        modules.emplace_back(std::make_unique<InputModule>());
        modules.emplace_back(std::make_unique<CameraModule>());
    }
}
#endif
