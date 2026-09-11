#pragma once

#include <math/math_vector.h>

#include "aiko_shape_desc.h"
#include "assets/types/mesh_asset.h"
#include "world/types/chunk_types.h"

namespace vw
{

    struct ChunkDataGenerationRequest
    {
        ChunkCoord coord = {0};
    };

    struct ChunkDataGenerationResponse
    {
        ChunkData data;
        aiko::MeshAsset asset;
        aiko::physics::TriangleMeshShapeDesc shape;
    };

}
