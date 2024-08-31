#pragma once

#include "aiko_types.h"
#include "constants.h"
#include "modules\render_module.h"
#include "modules\display_module.h"

namespace aiko::modules::builder
{
    AikoUPtr<aiko::RenderModule>  getRenderSystem (Aiko* aiko);
    AikoUPtr<aiko::DisplayModule> getDisplaySystem(Aiko* aiko);
}
