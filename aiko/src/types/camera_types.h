#pragma once

#include "aiko_types.h"

namespace aiko::camera
{
    enum CameraType { Perspective = 0, Orthographic };
    enum CameraController { Static, Orbit, Fly, Drag };

}