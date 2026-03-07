#pragma once

#include "aiko_types.h"
#include "aiko.h"
#include "constants.h"

#include <core/uuid.h>
#include <logger/logger.h>
#include <core/utils.h>

#include "events/event_dispatcher.hpp"

#include "components/camera_component.h"
#include "components/mesh_component.h"
#include "components/light_component.h"
#include "components/sprite_component.h"

#include "models/shader.h"
#include "models/mesh.h"
#include "models/camera.h"
#include "models/game_object.h"
#include "time/time.h"
#include "input/inputs_types.h"

#include "types/render_types.h"
#include "types/camera_controller_types.h"
#include "types/texture_types.h"
#include "types/color.h"

#include "systems/render_system.h"
#include "systems/input_system.h"

#include "core/singleton.h"
#include "math/math.h"
