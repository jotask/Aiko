#pragma once

#include "aiko_types.h"
#include "aiko.h"
#include "constants.h"

#include "core/uuid.h"
#include "core/log.h"
#include "core/utils.h"

#include "events/event_dispatcher.hpp"

#include "components/camera_component.h"
#include "components/mesh_component.h"
#include "components/light_component.h"
#include "components/texture_component.h"
#include "components/pbo_texture_component.h"
#include "components/grid_component.h"
#include "components/2d/sprite_component.h"

#include "models/shader.h"
#include "models/mesh.h"
#include "models/camera.h"
#include "models/game_object.h"
#include "models/time.h"
#include "models/input.h"

#include "types/render_types.h"
#include "types/camera_types.h"
#include "types/textures.h"
#include "types/color.h"

#include "systems/camera_system.h"
#include "systems/render_system.h"
#include "systems/input_system.h"
#include "systems/entity_component_system.h"

#include "shared/singleton.h"
#include "shared/math.h"
