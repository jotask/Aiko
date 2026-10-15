#pragma once
#include "aiko_body_desc.h"
#include "assets/types/mesh_asset.h"
#include "models/material.h"

namespace aiko::lab
{
    physics::BodyDesc boxDesc(vec3 position, vec3 halfExtent, physics::MotionType motion = physics::MotionType::Dynamic);
    MeshAsset visualMesh(const physics::ShapeDesc& shape);
    void configureMaterial(Material& material, const AssetId& shader, Color color);

}
