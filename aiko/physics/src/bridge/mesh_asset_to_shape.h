#pragma once

#include <aiko_shape_desc.h>

#include "assets/types/mesh_asset.h"

namespace aiko::physics
{

    TriangleMeshShapeDesc makeTriangleMeshShapeDesc( const MeshAsset& asset );

}
