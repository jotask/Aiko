#pragma once

#include <assets/types/mesh_asset.h>

namespace aiko::mesh
{
    // Calculate tangent.xyz and handedness in tangent.w.
    // A zero tangent indicates missing or unusable tangent data.
    vector<vec4> calculateTangents(const MeshAsset& asset);
}
